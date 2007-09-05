/*
 * core.cpp
 *
 * Núcleo do sistema OBF.
 *
 * Copyright (c) 2007 Paulo Matias.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cctype>

extern "C" {
#include <unistd.h>
}

#include "crypto/digest.h"
#include "crypto/hmac.h"
#include "network/postfile.h"
#include "network/browser.h"
#include "sqlite/sql.h"
#include "core.h"

using namespace std;


int get_current_year() {
    time_t tstamp = time(0);
    struct tm *tm_info = localtime(&tstamp);
    return 1900 + tm_info->tm_year;
}

static int getnumber_cb(void *arg, int argc, char **argv, char **colname) {
    int &n = *(int *)arg;
    if((argc == 1) && (argv[0]))
        n = atoi(argv[0]);
    return 0;
}

int Core::DiferencaAnos() {
    if(!db()) {
        throw("Não há planilha aberta.");
    }

    int ano_atual = get_current_year();
    int ano_arquivo = ano_atual;

    db()->Exec("select valor from info where campo='ano';", getnumber_cb, &ano_arquivo);

    return ano_atual - ano_arquivo;
}


void Core::Novo(const char *filename, const char *escola, const char *id, const char *cidade, const char *estado,
                const char *professor, const char *email, bool houve_primeira_fase) {

    close_db();
    remove(filename);

    sql_db = new SQL(filename);

    db()->Exec("create table info (campo text primary key, valor text);", 0, 0);
    db()->Exec("create table alunos (id integer primary key autoincrement, nome text not null,"
               "serie integer not null, nota integer check(nota between 0 and 20), email text, ddd text(2),"
               "telefone text, endereco text, cep text(8), cidade text, estado text(2),"
               "unique(nome,email) on conflict fail);", 0, 0);

    db()->Exec("insert into info values ('escola', '%q');", 0, 0, escola);
    db()->Exec("insert into info values ('id', '%q');", 0, 0, id);
    db()->Exec("insert into info values ('cidade', '%q');", 0, 0, cidade);
    db()->Exec("insert into info values ('estado', '%q');", 0, 0, estado);
    db()->Exec("insert into info values ('professor', '%q');", 0, 0, professor);
    db()->Exec("insert into info values ('email', '%q');", 0, 0, email);
    db()->Exec("insert into info values ('primeira-fase', '%q');", 0, 0, houve_primeira_fase ? "sim" : "nao");
    db()->Exec("insert into info values ('ano', '%d');", 0, 0, get_current_year());

    primeira_fase = houve_primeira_fase;
}


static int primeira_fase_cb(void *arg, int argc, char **argv, char **colname) {
    bool &primeira_fase = *(bool *)arg;
    primeira_fase = ((argc == 1) && (argv[0])) ? (!strcmp(argv[0], "sim")) : true;
    return 0;
}

void Core::Abrir(const char *filename) {
    close_db();

    FILE *fp = fopen(filename, "rb");
    if(fp) {
        fclose(fp);
    }
    else {
        throw("Não foi possível abrir o arquivo.");
    }

    sql_db = new SQL(filename);
    db()->Exec("select valor from info where campo='primeira-fase';", primeira_fase_cb, &primeira_fase);
}


struct SalvarAutenticado_data {
    FILE *fp;
    Crypto::HMAC &hmac;
};

static int SalvarAutenticado_cb(void *arg, int argc, char **argv, char **colname) {
    SalvarAutenticado_data &cb_data = *(SalvarAutenticado_data *)arg;
    ostringstream linha;

    linha << (argv[0] ? argv[0] : "");
    for(int i = 1; i < argc; i++) {
        linha << "\t" << (argv[i] ? argv[i] : "");
    }

    linha << "\n";

    cb_data.hmac << linha.str();
    fwrite(linha.str().data(), 1, linha.str().size(), cb_data.fp);

    return 0;
}

// Para o compilador saber qual versão de toupper() usar no transform().
static inline int upper(int c) {
    return toupper((unsigned char)c);
}

void Core::SalvarAutenticado(const char *filename, const char *senha) {
    if(!db()) {
        throw("Não há planilha aberta.");
    }

    if(!primeira_fase) {
        int series[] = { 8, 1, 2, 3, 0 };
        for(int *serie = series; *serie; serie++) {
            int num_alunos = 0;
            db()->Exec("select count(*) from alunos where serie=%d;", getnumber_cb, &num_alunos, *serie);
	    if(num_alunos > MAX_ALUNOS_SEM_PRIMEIRA_FASE) {
                throw("O número de alunos cadastrados ultrapassou o máximo de alunos permitido pelo\n"
                      "regulamento da OBF para uma escola que não participe da primera fase.\n"
                      "Remova alguns alunos e tente novamente.");
            }
        }
    }

    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        throw("Não foi possível abrir o arquivo para escrita.");
    }

    string s(senha);
    transform(s.begin(), s.end(), s.begin(), upper);

    Crypto::SHA512 algo;
    Crypto::HMAC hmac(algo, s);

    SalvarAutenticado_data cb_data = { fp, hmac };

    const char *info_footer = "------------------------------\n";
    const char *auth_banner = "\n---- AUTENTICAÇÃO DIGITAL ----\n";


    db()->Exec("select campo, valor from info;",
               SalvarAutenticado_cb, &cb_data);

    hmac << info_footer;
    fputs(info_footer, fp);

    db()->Exec("select nome, serie, nota, email, ddd, telefone, "
               "endereco, cep, cidade, estado from alunos;",
               SalvarAutenticado_cb, &cb_data);

    string auth_footer;

    auth_footer  = auth_banner;
    hmac >> auth_footer;
    auth_footer += auth_banner;

    fwrite(auth_footer.data(), 1, auth_footer.size(), fp);

    fclose(fp);

}


void Core::EnviarInternet(const char *senha, void(*progresso)(void *cb_data, float p), void *cb_data) {
    if(!db()) {
        throw("Não há planilha aberta.");
    }

#ifdef WIN32
    char *tmpfilename = tempnam(".", "obf");
    if(!tmpfilename) {
        throw("Não foi possível criar um arquivo temporário.");
    }
#else
    char tmpfilename[] = "obfXXXXXX";
    int fd = mkstemp(tmpfilename);
    if(fd < 0) {
        throw("Não foi possível criar um arquivo temporário.");
    }
    close(fd);
#endif

    try {
        SalvarAutenticado(tmpfilename, senha);
    }
    catch(const char *errmsg) {
        remove(tmpfilename);
        throw(errmsg);
    }

    const char *servidor = "www.sbfisica.org.br";
    const int porta = 80;
    const char *caminho = "/~matias/index.php";
    const char *campo = "arq";
    string response;

    FILE *fp = fopen(tmpfilename, "rb");

    try {
        Network::PostFile(fp, "obf.txt", campo, servidor, porta, caminho, response, progresso, cb_data);

        const char *erro_recepcao = "Erro ao receber informações do servidor. Tente novamente.";

        // Checa por mensagem de erro.
        const char *div_erro = "<div id=\"erro\">";
        string::size_type begin_err = response.find(div_erro, 0);
        if(begin_err != string::npos) {
            string::size_type end_err = response.find("</div>", begin_err);
            if(end_err == string::npos) {
                throw(erro_recepcao);
            }

            response[end_err] = 0;
            const char *msg_erro = response.data() + begin_err + strlen(div_erro);
            throw(msg_erro);
        }

        // Obtém a url do comprovante de envio.
        const char *loc_hdr = "\nLocation: ";
        string::size_type begin_loc = response.find(loc_hdr, 0);
        string::size_type end_loc = response.find_first_of("\r\n", begin_loc + 1);
        if((begin_loc == string::npos) || (end_loc == string::npos)) {
            throw(erro_recepcao);
        }
        response[end_loc] = 0;
        const char *url_comprovante = response.data() + begin_loc + strlen(loc_hdr);

        if(!Network::CallBrowser(url_comprovante)) {
            throw("Os dados foram enviados com sucesso, mas não foi possível exibir o comprovante.");
        }

    }
    catch(const char *errmsg) {
        fclose(fp);
        remove(tmpfilename);
        throw(errmsg);
    }

    fclose(fp);
    remove(tmpfilename);
#ifdef WIN32
    free(tmpfilename);
#endif
}



void Tabela_Alunos::init_dimensions() {
    Fl_SQL_Table::init_dimensions(10);
}

const char *Tabela_Alunos::get_colname(int i) {
    static char *colnames[] = { "Nome", "Série", "Nota", "E-mail", "DDD", "Telefone", "Endereço", "CEP", "Cidade", "Estado" };
    return colnames[i];
}

void Tabela_Alunos::query_data(int rowstart, int rowlen, sqlite3_callback cb, void *cbarg) {
    core.db()->Exec("select * from alunos limit %d,%d;", cb, cbarg, rowstart, rowlen);
}

void Tabela_Alunos::query_numrows(sqlite3_callback cb, void *cbarg) {
    core.db()->Exec("select count(*) from alunos;", cb, cbarg);
}

Tabela_Alunos::Tabela_Alunos(Core &mycore, int x, int y, int w, int h, const char *l) :Fl_SQL_Table(x,y,w,h,1,l),core(mycore) {
    rows(0); cols(10);
    col_width(0, 350);
    col_width(1, 50);
    col_width(2, 50);
    col_width(3, 200);
    col_width(4, 45);
    col_width(5, 95);
    col_width(6, 400);
    col_width(7, 95);
    col_width(8, 150);
    col_width(9, 60);
}

