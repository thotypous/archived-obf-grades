/*
 * core.h
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

#ifndef __CORE_H__
#define __CORE_H__

#include "sqlite/sql.h"
#include "FL/Fl_SQL_Table.H"


enum { MAX_ALUNOS_SEM_PRIMEIRA_FASE = 3 };


int get_current_year();

class Core {
    private:
        SQL *sql_db;
        void close_db() {
            if(sql_db)
                delete sql_db;
            sql_db = 0;
        }
    public:
        bool primeira_fase;

        Core() {
            sql_db = 0;
        }
        ~Core() {
            close_db();
        }
        SQL* db() const {
            return sql_db;
        }
        void Novo(const char *filename, const char *escola, const char *id, const char *cidade, const char *estado,
                  const char *professor, const char *email, bool houve_primeira_fase);
        void Abrir(const char *filename);
        void SalvarAutenticado(const char *filename, const char *senha);
        void EnviarInternet(const char *senha, void(*progresso)(void *cb_data, float p) = 0, void *cb_data = 0);
        int DiferencaAnos(); // ano atual - ano do arquivo aberto
};


class Tabela_Alunos : public Fl_SQL_Table {
    private:
        Core &core;
    protected:
        const char *get_colname(int i);
        void query_data(int rowstart, int rowlen, sqlite3_callback cb, void *cbarg);
        void query_numrows(sqlite3_callback cb, void *cbarg);
    public:
        Tabela_Alunos(Core &mycore, int x, int y, int w, int h, const char *l=0);
        void init_dimensions();
};

#endif
