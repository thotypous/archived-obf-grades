/*
 * gui.cpp
 *
 * Interface gráfica do sistema OBF.
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "gui.h"
#include "core.h"
#include "img/images.h"

#ifdef WIN32
#include <windows.h>
#else
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#endif

#include <FL/Fl_Progress.H>

using namespace std;


static Fl_Menu_Item menu_Serie[] = {
 {"", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"8", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"1", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"2", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"3", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

static Fl_Menu_Item menu_Estado[] = {
 {"", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"AC", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"AL", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"AP", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"AM", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"BA", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"CE", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"DF", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"ES", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"GO", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"MA", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"MT", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"MS", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"MG", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"PA", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"PB", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"PR", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"PE", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"PI", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"RJ", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"RN", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"RS", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"RO", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"RR", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"SC", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"SP", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"SE", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"TO", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

static int menu_item_position_from_value(const char *value, Fl_Menu_Item items[]) {
    for(int i = 0; items[i].text; i++) {
        if(!strcasecmp(items[i].text, value)) {
            return i;
        }
    }
    return 0;
}

static const char *filter_obf = "Dados da OBF (*.obf)\0*.obf\0\0";
static const char *filter_txt = "Arquivos de texto (*.txt)\0*.txt\0\0";

static char *put_extension(char *filename, const char *filter) {
    if(!filename)
        return 0;
    filter += strlen(filter) + 2; // go to extension .ext
    if((strlen(filename) < 4) || (strcasecmp(filename + strlen(filename) - 4, filter)))
        strcat(filename, filter);
    return filename;
}

static char *file_chooser(bool save_dialog = false, const char *filter = filter_obf) {
#ifdef WIN32
    OPENFILENAME ofn;
    static char filename[_MAX_PATH+1];

    filename[0] = 0;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrTitle = save_dialog ? "Salvar arquivo" : "Abrir arquivo";
    ofn.Flags = OFN_HIDEREADONLY | (save_dialog ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);

    if(save_dialog) {
        if(GetSaveFileName(&ofn))
            return put_extension(filename, filter);
    }
    else {
        if(GetOpenFileName(&ofn))
            return filename;
    }

    return 0;
#else
    char *filename = fl_file_chooser(save_dialog ? "Salvar arquivo" : "Abrir arquivo", filter, 0);
    if(save_dialog)
        return put_extension(filename, filter);
    return filename;
#endif
}

static void alert_msg(const char *msg) {
#ifdef WIN32
    MessageBox(0, msg, "Sistema OBF", MB_ICONEXCLAMATION);
#else
    fl_alert(msg);
#endif
}

static bool ask_msg(const char *msg) {
#ifdef WIN32
    return (MessageBox(0, msg, "Sistema OBF", MB_ICONQUESTION | MB_YESNO) == IDYES);
#else
    return (fl_choice(msg, NULL, "Sim", "Não") == 1);
#endif
}


static void botao_novo_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;
    janela.janela_info_escola->novo_arquivo = true;
    janela.janela_info_escola->show();
}

static void botao_abrir_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;
    char *filename = file_chooser();
    if(filename) {
        try {
            janela.core.Abrir(filename);
            janela.grupo_botoes_acao->activate();
            janela.grupo_formulario->activate();
            janela.tabela_alunos->activate();
            janela.tabela_alunos->init_dimensions();
            janela.tabela_alunos->clear_cache();
            janela.tabela_alunos->redraw();

            if(janela.core.primeira_fase) {
                janela.campo_nota->value("");
                janela.campo_nota->activate();
            }
            else {
                janela.campo_nota->value("0");
                janela.campo_nota->deactivate();
            }

            int dif_anos = janela.core.DiferencaAnos();
            if(dif_anos > 0) {
                ostringstream msg;
                msg << "O sistema detectou que o arquivo que está sendo aberto é " << dif_anos << " ano" <<
                       ((dif_anos > 1) ? "s" : "") << " mais\n"
                       "antigo que a data atual. Deseja atualizar automaticamente as séries dos\n"
                       "alunos registrados nesse arquivo e apagar automaticamente alunos que\n"
                       "já devem ter concluído o Ensino Médio?";
                if(ask_msg(msg.str().data())) {
                    SQL *db = janela.core.db();
                    db->Exec("update alunos set serie=0 where serie=8;", 0, 0);
                    db->Exec("update alunos set serie=serie+%d;", 0, 0, dif_anos);
                    db->Exec("delete from alunos where serie > 3;", 0, 0);
                    db->Exec("update info set valor='%d' where campo='ano';", 0, 0, get_current_year());
                    janela.tabela_alunos->init_dimensions();
                    janela.tabela_alunos->clear_cache();
                    janela.tabela_alunos->redraw();
                    alert_msg("Arquivo de dados atualizado com sucesso.");
                }
            }
        }
        catch(const char *errmsg) {
            alert_msg(errmsg);
        }
    }
}

static int info_escola_carregar_cb(void *arg, int argc, char **argv, char **colname) {
    Janela_Info_Escola &info = *(Janela_Info_Escola *)arg;
    const char *campo = argv[0];
    const char *valor = argv[1];

    if(!strcmp(campo, "escola"))
        info.campo_nome->value(valor);
    else if(!strcmp(campo, "id"))
        info.campo_id->value(valor);
    else if(!strcmp(campo, "cidade"))
        info.campo_cidade->value(valor);
    else if(!strcmp(campo, "estado"))
        info.campo_estado->value(menu_item_position_from_value(valor, menu_Estado));
    else if(!strcmp(campo, "professor"))
        info.campo_professor->value(valor);
    else if(!strcmp(campo, "email"))
        info.campo_email->value(valor);
    else if(!strcmp(campo, "primeira-fase")) {
        bool v = !strcmp(valor, "sim");
        info.primeira_fase_sim->value(v);
        info.primeira_fase_nao->value(!v);
    }

    return 0;
}

static void botao_info_escola_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;
    Janela_Info_Escola *info = janela.janela_info_escola;

    try {
        info->novo_arquivo = false;
        janela.core.db()->Exec("select campo, valor from info;", info_escola_carregar_cb, info);
        info->show();
    }
    catch(const char *errmsg) {
        alert_msg(errmsg);
    }
}

static void botao_enviar_internet_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;
    janela.janela_enviar_internet->inicializar_campos();
    janela.janela_enviar_internet->show();
}

static void botao_enviar_disquete_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;
    janela.janela_enviar_disquete->inicializar_campos();
    janela.janela_enviar_disquete->show();
}


bool Janela_Principal::validar_formulario() {
    if(campo_nome->size() == 0) {
        alert_msg("Digite o nome do aluno.");
        campo_nome->take_focus();
        return false;
    }

    if(campo_serie->value() == 0) {
        alert_msg("Selecione a série do aluno.");
        campo_serie->take_focus();
        return false;
    }

    if(campo_nota->size() == 0) {
        alert_msg("Digite a nota do aluno.");
        campo_nota->take_focus();
        return false;
    }

    if((campo_email->size() != 0) && (!strstr(campo_email->value(), "@"))) {
        alert_msg("E-mail inválido.");
        campo_email->take_focus();
        return false;
    }

    if((campo_ddd->size() != 0) && (campo_ddd->size() != 2)) {
        alert_msg("DDD inválido.");
        campo_ddd->take_focus();
        return false;
    }

    if((campo_telefone->size() != 0) && (campo_telefone->size() < 7)) {
        alert_msg("Telefone inválido.");
        campo_telefone->take_focus();
        return false;
    }

    if((campo_cep->size() != 0) && (campo_cep->size() != 8)) {
        alert_msg("CEP inválido.");
        campo_cep->take_focus();
        return false;
    }

    return true;
}

static const char *explain_errmsg(const char *msg) {
    // Esta função realiza traduções para erros que possam acontecer mais comumente no SQL.
    if(!strcmp(msg, "constraint failed")) {
        return "A nota do aluno deve estar entre 0 e 20.\n"
               "Corrija a nota no formulário e tente novamente.";
    }
    if(!strcmp(msg, "columns nome, email are not unique")) {
        return "O aluno que você está adicionando já foi incluído antes no formulário.\n"
               "Já existe um aluno no formulário com mesmo nome e mesmo e-mail.\n"
               "Estamos prevenindo que seja inserida uma entrada duplicada.";
    }
    return msg;
}

static void botao_adicionar_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;

    if(!janela.validar_formulario())
        return;

    try {
        janela.core.db()->Exec("insert into alunos (nome, serie, nota, email, ddd, telefone, endereco, cep, "
                               "cidade, estado) values ('%q', %q, %q, '%q', '%q', '%q', '%q', '%q', '%q', '%q');",
                               0, 0, janela.campo_nome->value(), menu_Serie[janela.campo_serie->value()].text,
                               janela.campo_nota->value(), janela.campo_email->value(), janela.campo_ddd->value(),
                               janela.campo_telefone->value(), janela.campo_endereco->value(),
                               janela.campo_cep->value(), janela.campo_cidade->value(),
                               menu_Estado[janela.campo_estado->value()].text);

        janela.tabela_alunos->rows_changed(+1);

        if(janela.selected_id)
            free(janela.selected_id);

        janela.selected_id = (char *)malloc(16);
        snprintf(janela.selected_id, 16, "%lld", janela.core.db()->last_insert_rowid());
    }
    catch(const char *errmsg) {
        alert_msg(explain_errmsg(errmsg));
    }

    janela.campo_nome->position(0, janela.campo_nome->size());
    janela.campo_nome->take_focus();
}

static void botao_mudar_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;

    if((!janela.selected_id) || (!janela.validar_formulario()))
        return;

    try {
        janela.core.db()->Exec("update alunos set nome='%q', serie=%q, nota=%q, email='%q', ddd='%q', telefone='%q', "
                               "endereco='%q', cep='%q', cidade='%q', estado='%q' where id=%q;",
                               0, 0, janela.campo_nome->value(), menu_Serie[janela.campo_serie->value()].text,
                               janela.campo_nota->value(), janela.campo_email->value(), janela.campo_ddd->value(),
                               janela.campo_telefone->value(), janela.campo_endereco->value(),
                               janela.campo_cep->value(), janela.campo_cidade->value(),
                               menu_Estado[janela.campo_estado->value()].text, janela.selected_id);

        janela.tabela_alunos->clear_cache();
        janela.tabela_alunos->redraw();
    }
    catch(const char *errmsg) {
        alert_msg(explain_errmsg(errmsg));
    }

    janela.campo_nome->position(0, janela.campo_nome->size());
    janela.campo_nome->take_focus();
}

static void botao_remover_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;

    if(janela.selected_id) {
        try {
            janela.core.db()->Exec("delete from alunos where id=%q;", 0, 0, janela.selected_id);
            free(janela.selected_id);
            janela.selected_id = 0;
            janela.tabela_alunos->rows_changed(-1);
        }
        catch(const char *errmsg) {
            alert_msg(errmsg);
        }
    }
}

static void tabela_alunos_cb(Fl_Widget *o, void *cb_data) {
    Janela_Principal &janela = *(Janela_Principal *)cb_data;
    Tabela_Alunos &tabela = *janela.tabela_alunos;

    if(tabela.callback_context() == 0x10) {
        int row = tabela.callback_row();

        if(janela.selected_id)
            free(janela.selected_id);

        if(row >= 0) {
            janela.selected_id = strdup(tabela.get_cell_value(row, 0));
            janela.campo_nome->value(tabela.get_cell_value(row, 1));
            janela.campo_serie->value(menu_item_position_from_value(tabela.get_cell_value(row, 2), menu_Serie));
            janela.campo_nota->value(tabela.get_cell_value(row, 3));
            janela.campo_email->value(tabela.get_cell_value(row, 4));
            janela.campo_ddd->value(tabela.get_cell_value(row, 5));
            janela.campo_telefone->value(tabela.get_cell_value(row, 6));
            janela.campo_endereco->value(tabela.get_cell_value(row, 7));
            janela.campo_cep->value(tabela.get_cell_value(row, 8));
            janela.campo_cidade->value(tabela.get_cell_value(row, 9));
            janela.campo_estado->value(menu_item_position_from_value(tabela.get_cell_value(row, 10), menu_Estado));
        }
        else {
            janela.selected_id = 0;
        }
    }

}


Janela_Principal::Janela_Principal() :Fl_Window(25, 25, 785, 545, "Olimpíada Brasileira de Física") {

    selected_id = 0;

    grupo_botoes_arquivo = new Fl_Group(10, 10, 185, 85);

    {
        Fl_Button *o = new Fl_Button(10, 10, 90, 85, "&Novo arquivo de dados");
        o->tooltip("Cria um novo arquivo de dados para armazenar os dados inseridos no sistema.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_empty);
        o->labelsize(9);
        o->align(FL_ALIGN_WRAP);
        o->callback(botao_novo_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(105, 10, 90, 85, "A&brir arquivo de dados");
        o->tooltip("Abre um arquivo de dados criado anteriormente para continuar a trabalhar no mesmo.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_open);
        o->labelsize(9);
        o->align(FL_ALIGN_WRAP);
        o->callback(botao_abrir_cb, this);
        o->when(FL_WHEN_RELEASE);
    }

    grupo_botoes_arquivo->end();


    grupo_botoes_acao = new Fl_Group(220, 10, 300, 85);
    grupo_botoes_acao->deactivate();

    {
        Fl_Button *o = new Fl_Button(220, 10, 90, 85, "Informações da &escola");
        o->tooltip("Abre a tela de edição das informações da escola.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_escola);
        o->labelsize(9);
        o->align(FL_ALIGN_WRAP);
        o->callback(botao_info_escola_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(335, 10, 90, 85, "Enviar via &Internet");
        o->tooltip("Envia os dados digitados para a SBF via Internet.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_internet);
        o->labelsize(9);
        o->align(FL_ALIGN_WRAP);
        o->callback(botao_enviar_internet_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(430, 10, 90, 85, "Enviar via &disquete");
        o->tooltip("Gera um arquivo para ser enviado por correio à SBF via disquete ou CD-ROM, ou para ser enviado "
                   "via Internet a partir de um navegador Web.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_disquete);
        o->labelsize(9);
        o->align(FL_ALIGN_WRAP);
        o->callback(botao_enviar_disquete_cb, this);
        o->when(FL_WHEN_RELEASE);
    }

    grupo_botoes_acao->end();


    grupo_formulario = new Fl_Group(10, 105, 765, 205);
    grupo_formulario->deactivate();

    {
        Fl_Box *o = new Fl_Box(10, 105, 765, 135);
        o->box(FL_ENGRAVED_FRAME);
    }
    {
        Fl_Input *o = campo_nome = new Fl_Input(85, 115, 445, 25, "Nome:");
        o->box(FL_ENGRAVED_BOX);
        o->labelfont(1);
        o->labelsize(13);
        o->textsize(13);
    }
    {
        Fl_Choice *o = campo_serie = new Fl_Choice(585, 115, 55, 25, "Série:");
        o->box(FL_EMBOSSED_BOX);
        o->down_box(FL_FLAT_BOX);
        o->labelfont(1);
        o->labelsize(13);
        o->textsize(13);
        o->menu(menu_Serie);
    }
    {
        Fl_Input *o = campo_nota = new Fl_Input(700, 115, 65, 25, "Nota:");
        o->type(2);
        o->box(FL_ENGRAVED_BOX);
        o->labelfont(1);
        o->labelsize(13);
        o->textsize(13);
    }
    {
        Fl_Input *o = campo_email = new Fl_Input(85, 145, 375, 25, "E-mail:");
        o->box(FL_ENGRAVED_BOX);
        o->labelsize(13);
        o->textsize(13);
    }
    {
        new Fl_Box(470, 146, 82, 23, "Telefone: (");
        Fl_Input *o = campo_ddd = new Fl_Input(555, 145, 35, 25);
        o->type(2);
        o->box(FL_ENGRAVED_BOX);
        o->labelsize(13);
        o->textsize(13);
        o->maximum_size(2);
        new Fl_Box(591, 146, 15, 23, ")");
    }
    {
        Fl_Input *o = campo_telefone = new Fl_Input(610, 145, 155, 25);
        o->type(2);
        o->box(FL_ENGRAVED_BOX);
        o->labelsize(13);
        o->textsize(13);
    }
    {
        Fl_Input *o = campo_endereco = new Fl_Input(85, 175, 680, 25, "Endereço:");
        o->box(FL_ENGRAVED_BOX);
        o->labelsize(13);
        o->textsize(13);
    }
    {
        Fl_Input *o = campo_cep = new Fl_Input(85, 205, 105, 25, "CEP:");
        o->type(2);
        o->box(FL_ENGRAVED_BOX);
        o->labelsize(13);
        o->textsize(13);
        o->maximum_size(8);
    }
    {
        Fl_Input *o = campo_cidade = new Fl_Input(260, 205, 380, 25, "Cidade:");
        o->box(FL_ENGRAVED_BOX);
        o->labelsize(13);
        o->textsize(13);
    }
    {
        Fl_Choice *o = campo_estado = new Fl_Choice(710, 205, 55, 25, "Estado:");
        o->box(FL_EMBOSSED_BOX);
        o->down_box(FL_FLAT_BOX);
        o->labelsize(13);
        o->textsize(13);
        o->menu(menu_Estado);
    }

    {
        Fl_Return_Button *o = new Fl_Return_Button(10, 255, 85, 55, "&Adicionar");
        o->tooltip("Adiciona os dados que estão escritos no formulário a uma nova linha na tabela de alunos.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_insert_table_row);
        o->labelsize(9);
        o->callback(botao_adicionar_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(100, 255, 65, 55, "A&lterar");
        o->tooltip("Muda os dados da linha selecionada na tabela de alunos para os que estão escritos no formulário.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_write_paper);
        o->labelsize(9);
        o->callback(botao_mudar_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(170, 255, 65, 55, "&Remover");
        o->tooltip("Remove a linha selecionada da tabela de alunos.");
        o->box(FL_THIN_UP_BOX);
        o->image(image_delete_table_row);
        o->labelsize(9);
        o->callback(botao_remover_cb, this);
        o->when(FL_WHEN_RELEASE);
    }

    {
        Fl_Output *o = area_dica = new Fl_Output(250, 258, 525, 50);
        o->type(12);
        o->box(FL_NO_BOX);
        o->textsize(11);
        o->deactivate();
        o->value("DICA: Use a tecla Tab para alternar entre os campos e a tecla Enter\n"
                 "para adicionar um novo aluno. Ao adicionar um novo aluno, o\n"
                 "sistema voltará ao campo Nome.");
    }

    grupo_formulario->end();


    {
        Tabela_Alunos *o = tabela_alunos = new Tabela_Alunos(core, 10, 315, 765, 220);
        o->callback(tabela_alunos_cb, this);
        o->when(FL_WHEN_CHANGED|FL_WHEN_RELEASE);
        o->deactivate();
        resizable(o);
    }

    end();

    janela_info_escola = new Janela_Info_Escola(*this);
    janela_enviar_internet = new Janela_Enviar_Internet(*this);
    janela_enviar_disquete = new Janela_Enviar_Disquete(*this);

}

Janela_Principal::~Janela_Principal() {
    delete janela_info_escola;
    delete janela_enviar_internet;
    delete janela_enviar_disquete;

    if(selected_id)
        free(selected_id);
}



static void botao_cancelar_cb(Fl_Widget *o, void *cb_data) {
    Fl_Window &janela = *(Fl_Window *)cb_data;
    janela.hide();
}

static void confirmar_info_escola_cb(Fl_Widget *o, void *cb_data) {
    Janela_Info_Escola &janela = *(Janela_Info_Escola *)cb_data;

    if(janela.campo_nome->size() == 0) {
        alert_msg("Digite o nome da escola.");
        janela.campo_nome->take_focus();
        return;
    }

    if(janela.campo_id->size() == 0) {
        alert_msg("Digite o código de identificação da escola.");
        janela.campo_id->take_focus();
        return;
    }

    if(janela.campo_cidade->size() == 0) {
        alert_msg("Digite a cidade na qual se localiza a escola.");
        janela.campo_cidade->take_focus();
        return;
    }

    if(janela.campo_estado->value() == 0) {
        alert_msg("Selecione o estado no qual se localiza a escola.");
        janela.campo_estado->take_focus();
        return;
    }

    if(janela.campo_professor->size() == 0) {
        alert_msg("Digite o nome do professor responsável.");
        janela.campo_professor->take_focus();
        return;
    }

    if((janela.campo_email->size() == 0) || (!strstr(janela.campo_email->value(), "@"))) {
        alert_msg("Digite o e-mail do professor responsável.");
        janela.campo_email->take_focus();
        return;
    }

    if((janela.primeira_fase_sim->value() == 0) && (janela.primeira_fase_nao->value() == 0)) {
        alert_msg("Diga se houve primeira fase da OBF ou não na escola.");
        janela.primeira_fase_sim->take_focus();
        return;
    }

    if(janela.novo_arquivo) {
        const char *filename = file_chooser(true);
        if(!filename) {
            return;
        }

        try {
            janela.parent.core.Novo(filename, janela.campo_nome->value(), janela.campo_id->value(),
                                    janela.campo_cidade->value(), menu_Estado[janela.campo_estado->value()].text,
                                    janela.campo_professor->value(), janela.campo_email->value(),
                                    janela.primeira_fase_sim->value() == 1);

            janela.parent.grupo_botoes_acao->activate();
            janela.parent.grupo_formulario->activate();
            janela.parent.tabela_alunos->activate();
            janela.parent.tabela_alunos->init_dimensions();
        }
        catch(const char *errmsg) {
            alert_msg(errmsg);
        }
    }
    else {
        SQL *db = janela.parent.core.db();

        janela.parent.core.primeira_fase = (janela.primeira_fase_sim->value() == 1);

        try {
            db->Exec("update info set valor='%q' where campo='escola';", 0, 0, janela.campo_nome->value());
            db->Exec("update info set valor=%q where campo='id';", 0, 0, janela.campo_id->value());
            db->Exec("update info set valor='%q' where campo='cidade';", 0, 0, janela.campo_cidade->value());
            db->Exec("update info set valor='%q' where campo='estado';", 0, 0, menu_Estado[janela.campo_estado->value()].text);
            db->Exec("update info set valor='%q' where campo='professor';", 0, 0, janela.campo_professor->value());
            db->Exec("update info set valor='%q' where campo='email';", 0, 0, janela.campo_email->value());
            db->Exec("update info set valor='%q' where campo='primeira-fase';", 0, 0, janela.parent.core.primeira_fase ? "sim" : "nao");
        }
        catch(const char *errmsg) {
            alert_msg(errmsg);
        }
    }

    if(janela.parent.core.primeira_fase) {
        janela.parent.campo_nota->value("");
        janela.parent.campo_nota->activate();
    }
    else {
        janela.parent.campo_nota->value("0");
        janela.parent.campo_nota->deactivate();
    }

    janela.hide();
}


Janela_Info_Escola::Janela_Info_Escola(Janela_Principal &my_parent)
:Fl_Window(25, 25, 535, 365, "Informações da Escola"), parent(my_parent) {

    {
        Fl_Group *o = new Fl_Group(10, 30, 515, 75, "Informações da Escola");
        o->box(FL_ENGRAVED_FRAME);
        o->labelfont(1);
        o->labelsize(16);
        {
            Fl_Input *o = campo_nome = new Fl_Input(80, 40, 310, 25, "Nome:");
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_id = new Fl_Input(425, 40, 90, 25, "ID:");
            o->tooltip("Código numérico de identificação da escola.");
            o->type(2);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_cidade = new Fl_Input(80, 70, 310, 25, "Cidade:");
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Choice *o = campo_estado = new Fl_Choice(460, 70, 55, 25, "Estado:");
            o->box(FL_EMBOSSED_BOX);
            o->down_box(FL_FLAT_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
            o->menu(menu_Estado);
        }

        o->end();
    }

    {
        Fl_Group *o = new Fl_Group(10, 140, 515, 75, "Professor Responsável");
        o->box(FL_ENGRAVED_FRAME);
        o->labelfont(1);
        o->labelsize(16);

        {
            Fl_Input *o = campo_professor = new Fl_Input(80, 150, 435, 25, "Nome:");
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_email = new Fl_Input(80, 180, 435, 25, "E-mail:");
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }

        o->end();
    }

    {
        Fl_Group *o = new Fl_Group(10, 245, 515, 70, "Processo de seleção");
        o->box(FL_ENGRAVED_FRAME);
        o->labelfont(1);
        o->labelsize(16);
        new Fl_Box(20, 253, 495, 24, "A escola participa da primeira fase?");

        {
            Fl_Group *o = new Fl_Group(205, 280, 125, 25);

            {
                Fl_Check_Button *o = primeira_fase_sim = new Fl_Check_Button(210, 280, 50, 25, "Sim");
                o->type(102);
                o->down_box(FL_PLASTIC_UP_BOX);
                o->color(FL_BACKGROUND2_COLOR);
            }
            {
                Fl_Check_Button *o = primeira_fase_nao = new Fl_Check_Button(275, 280, 50, 25, "Não");
                o->type(102);
                o->down_box(FL_PLASTIC_UP_BOX);
                o->color(FL_BACKGROUND2_COLOR);
            }

            o->end();
        }

        o->end();
    }

    {
        Fl_Return_Button *o = new Fl_Return_Button(130, 325, 130, 30, "Confirmar");
        o->box(FL_THIN_UP_BOX);
        o->callback(confirmar_info_escola_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(265, 325, 130, 30, "Cancelar");
        o->box(FL_THIN_UP_BOX);
        o->callback(botao_cancelar_cb, this);
        o->when(FL_WHEN_RELEASE);
    }

    set_modal();
    end();
}

static int enviar_carregar_cb(void *arg, int argc, char **argv, char **colname) {
    Janela_Enviar &janela = *(Janela_Enviar *)arg;
    const char *campo = argv[0];
    const char *valor = argv[1];

    if(!strcmp(campo, "id"))
        janela.campo_id->value(valor);
    else if(!strcmp(campo, "email"))
        janela.campo_email->value(valor);
    else if(!strcmp(campo, "ano"))
        janela.campo_ano->value(valor);

    return 0;
}

void Janela_Enviar::inicializar_campos() {
    try {
        parent.core.db()->Exec("select campo, valor from info;", enviar_carregar_cb, this);
    }
    catch(const char *errmsg) {
        alert_msg(errmsg);
    }
}

bool Janela_Enviar::validar_campos() {
    if(campo_senha->size() == 0) {
        alert_msg("Digite sua senha.");
        campo_senha->take_focus();
        return false;
    }

    if(strcmp(campo_senha->value(), campo_confirmar_senha->value())) {
        alert_msg("A senha e a confirmação de senha não conferem. Digite-as novamente.");
        campo_senha->position(0, campo_senha->size());
        campo_senha->take_focus();
        return false;
    }

    if((campo_email->size() == 0) || (!strstr(campo_email->value(), "@"))) {
        alert_msg("Digite o e-mail do professor responsável.");
        campo_email->take_focus();
        return false;
    }

    if(campo_id->size() == 0) {
        alert_msg("Digite o código de identificação da escola.");
        campo_id->take_focus();
        return false;
    }

    if(campo_ano->size() == 0) {
        alert_msg("Digite o ano atual.");
        campo_ano->take_focus();
        return false;
    }

    return true;
}

void Janela_Enviar::salvar_campos() {
    SQL *db = parent.core.db();
    db->Exec("update info set valor='%q' where campo='id';", 0, 0, campo_id->value());
    db->Exec("update info set valor='%q' where campo='email';", 0, 0, campo_email->value());
    db->Exec("update info set valor='%q' where campo='ano';", 0, 0, campo_ano->value());
}

class Progress_Dialog : public Fl_Window {
    public:
        Fl_Progress *bar;

        Progress_Dialog(const char *l = "Progresso") :Fl_Window(100, 100, 200, 25, l) {
            bar = new Fl_Progress(5, 5, 190, 15);
            bar->minimum(0.0f);
            bar->maximum(1.0f);
            bar->value(0.0f);
            bar->selection_color(FL_DARK_BLUE);

            set_modal();
            end();
            show();
        }
};

static void progresso_cb(void *cb_data, float p) {
    Fl_Progress *bar = (Fl_Progress *)cb_data;
    bar->value(p);
    Fl::check();
}

static void enviar_internet_cb(Fl_Widget *o, void *cb_data) {
    Janela_Enviar &janela = *(Janela_Enviar *)cb_data;
    if(!janela.validar_campos())
        return;
    try {
        janela.salvar_campos();

        Progress_Dialog dlg("Enviando");
        janela.parent.core.EnviarInternet(janela.campo_senha->value(), progresso_cb, dlg.bar);
        janela.hide();
    }
    catch(const char *errmsg) {
        alert_msg(errmsg);
    }
}

static void enviar_disquete_cb(Fl_Widget *o, void *cb_data) {
    Janela_Enviar &janela = *(Janela_Enviar *)cb_data;
    if(!janela.validar_campos())
        return;
    try {
        janela.salvar_campos();

        const char *filename = file_chooser(true, filter_txt);
        if(filename) {
            janela.parent.core.SalvarAutenticado(filename, janela.campo_senha->value());
            alert_msg("Arquivo salvo com sucesso.");
            janela.hide();
        }
    }
    catch(const char *errmsg) {
        alert_msg(errmsg);
    }
}


Janela_Enviar_Internet::Janela_Enviar_Internet(Janela_Principal &my_parent)
:Janela_Enviar(my_parent, 25, 25, 385, 435, "Enviar via Internet") {
    {
        Fl_Group *o = new Fl_Group(10, 8, 365, 64);
        o->box(FL_ENGRAVED_FRAME);
        {
            Fl_Box *o = new Fl_Box(25, 13, 50, 52);
            o->image(image_internet);
        }
        {
            Fl_Box *o = new Fl_Box(80, 18, 285, 42, "Através desta opção os dados serão enviados diretamente "
                                   "aos servidores da SBF. Por favor forneça a sua senha e confirme se os dados "
                                   "abaixo estão corretos.");
            o->labelsize(10);
            o->align(FL_ALIGN_WRAP);
        }
        o->end();
    }
    {
        Fl_Group *o = new Fl_Group(10, 81, 365, 76);
        o->box(FL_ENGRAVED_FRAME);
        {
            Fl_Input *o = campo_senha = new Fl_Input(160, 93, 205, 25, "Digite a senha:");
            o->type(5);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_confirmar_senha = new Fl_Input(160, 123, 205, 25, "Confirme a senha:");
            o->type(5);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        o->end();
    }
    {
        Fl_Group *o = new Fl_Group(10, 191, 365, 100, "CONFIRME COM ATENÇÃO");
        o->box(FL_ENGRAVED_FRAME);
        o->labelfont(1);
        o->labelsize(20);
        {
            Fl_Input *o = campo_email = new Fl_Input(170, 197, 195, 25, "E-mail do professor:");
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_id = new Fl_Input(170, 225, 195, 25, "ID da escola:");
            o->type(2);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_ano = new Fl_Input(170, 255, 195, 25, "Ano atual:");
            o->type(2);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        o->end();
    }
    {
        Fl_Group *o = new Fl_Group(10, 299, 365, 80);
        o->box(FL_ENGRAVED_FRAME);
        {
            Fl_Box *o = new Fl_Box(15, 299, 355, 80, "IMPORTANTE: Se dados de sua escola houverem sido previamente enviados, os "
                                                     "dados antigos serão apagados e substituídos pelos novos dados.");
            o->labelfont(1);
            o->labelsize(13);
            o->align(FL_ALIGN_WRAP);
        }
        o->end();
    }
    {
        Fl_Return_Button *o = new Fl_Return_Button(85, 390, 100, 35, "Enviar");
        o->box(FL_THIN_UP_BOX);
        o->callback(enviar_internet_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(195, 390, 100, 35, "Cancelar");
        o->box(FL_THIN_UP_BOX);
        o->callback(botao_cancelar_cb, this);
        o->when(FL_WHEN_RELEASE);
    }

    set_modal();
    end();
}



Janela_Enviar_Disquete::Janela_Enviar_Disquete(Janela_Principal &my_parent)
:Janela_Enviar(my_parent, 25, 25, 385, 460, "Enviar via disquete") {
    {
        Fl_Group *o = new Fl_Group(10, 8, 365, 94);
        o->box(FL_ENGRAVED_FRAME);
        {
            Fl_Box *o = new Fl_Box(25, 18, 50, 52);
            o->image(image_disquete);
        }
        {
            Fl_Box *o = new Fl_Box(80, 19, 285, 56, "Esta opção gera um arquivo autenticado digitalmente que pode ser enviado "
                                   "à SBF via correio dentro de um disquete ou CD-ROM. O arquivo pode também ser enviado por "
                                   "meio de um navegador Web pelo endereço:");
            o->labelsize(10);
            o->align(FL_ALIGN_WRAP);
        }
        {
            Fl_Box *o = new Fl_Box(15, 78, 350, 17, "http://www.sbfisica.org.br/~matias");
            o->labelsize(9);
            o->align(FL_ALIGN_WRAP);
        }
        o->end();
    }
    {
        Fl_Group *o = new Fl_Group(10, 106, 365, 76);
        o->box(FL_ENGRAVED_FRAME);
        {
            Fl_Input *o = campo_senha = new Fl_Input(160, 118, 205, 25, "Digite a senha:");
            o->type(5);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_confirmar_senha = new Fl_Input(160, 148, 205, 25, "Confirme a senha:");
            o->type(5);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        o->end();
    }
    {
        Fl_Group *o = new Fl_Group(10, 216, 365, 105, "CONFIRME COM ATENÇÃO");
        o->box(FL_ENGRAVED_FRAME);
        o->labelfont(1);
        o->labelsize(20);
        {
            Fl_Input *o = campo_email = new Fl_Input(170, 222, 195, 25, "E-mail do professor:");
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_id = new Fl_Input(170, 255, 195, 25, "ID da escola:");
            o->type(2);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        {
            Fl_Input *o = campo_ano = new Fl_Input(170, 285, 195, 25, "Ano atual:");
            o->type(2);
            o->box(FL_ENGRAVED_BOX);
            o->labelfont(1);
            o->labelsize(13);
            o->textsize(13);
        }
        o->end();
    }
    {
        Fl_Group *o = new Fl_Group(10, 329, 365, 77);
        o->box(FL_ENGRAVED_FRAME);
        {
            Fl_Box *o = new Fl_Box(15, 329, 355, 77, "IMPORTANTE: Assegure que todos os dados acima estejam corretos, caso "
                                   "contrário a SBF não será capaz de comprovar a autenticidade do arquivo que será enviado.");
            o->labelfont(1);
            o->labelsize(13);
            o->align(FL_ALIGN_WRAP);
        }
        o->end();
    }
    {
        Fl_Return_Button *o = new Fl_Return_Button(85, 415, 100, 35, "Gerar");
        o->box(FL_THIN_UP_BOX);
        o->callback(enviar_disquete_cb, this);
        o->when(FL_WHEN_RELEASE);
    }
    {
        Fl_Button *o = new Fl_Button(195, 415, 100, 35, "Cancelar");
        o->box(FL_THIN_UP_BOX);
        o->callback(botao_cancelar_cb, this);
        o->when(FL_WHEN_RELEASE);
    }

    set_modal();
    end();
}

