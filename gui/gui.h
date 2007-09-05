/*
 * gui.h
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

#ifndef gui_h
#define gui_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Output.H>

#include "core.h"


class Janela_Info_Escola;
class Janela_Enviar_Internet;
class Janela_Enviar_Disquete;

class Janela_Principal : public Fl_Window {
    public:
        Core core;

        Fl_Group *grupo_botoes_arquivo;
        Fl_Group *grupo_botoes_acao;

        Fl_Group *grupo_formulario;
        Fl_Input *campo_nome;
        Fl_Choice *campo_serie;
        Fl_Input *campo_nota;
        Fl_Input *campo_email;
        Fl_Input *campo_ddd;
        Fl_Input *campo_telefone;
        Fl_Input *campo_endereco;
        Fl_Input *campo_cep;
        Fl_Input *campo_cidade;
        Fl_Choice *campo_estado;
        Fl_Output *area_dica;

        Tabela_Alunos *tabela_alunos;

        Janela_Info_Escola *janela_info_escola;
        Janela_Enviar_Internet *janela_enviar_internet;
        Janela_Enviar_Disquete *janela_enviar_disquete;

        char *selected_id;

        Janela_Principal();
        ~Janela_Principal();

        bool validar_formulario();
};

class Janela_Info_Escola : public Fl_Window {
    public:
        Janela_Principal &parent;

        bool novo_arquivo;

        Fl_Input *campo_nome;
        Fl_Input *campo_id;
        Fl_Input *campo_cidade;
        Fl_Choice *campo_estado;

        Fl_Input *campo_professor;
        Fl_Input *campo_email;

        Fl_Check_Button *primeira_fase_sim;
        Fl_Check_Button *primeira_fase_nao;

        Janela_Info_Escola(Janela_Principal &my_parent);
};

class Janela_Enviar : public Fl_Window {
    public:
        Janela_Principal &parent;

        Fl_Input *campo_senha;
        Fl_Input *campo_confirmar_senha;
        Fl_Input *campo_email;
        Fl_Input *campo_id;
        Fl_Input *campo_ano;

        Janela_Enviar(Janela_Principal &my_parent, int x, int y, int w, int h, const char *l=0)
        :Fl_Window(x,y,w,h,l), parent(my_parent) {
        }

        void inicializar_campos();
        bool validar_campos();
        void salvar_campos();
};

class Janela_Enviar_Internet : public Janela_Enviar {
    public:
        Janela_Enviar_Internet(Janela_Principal &my_parent);
};

class Janela_Enviar_Disquete : public Janela_Enviar {
    public:
        Janela_Enviar_Disquete(Janela_Principal &my_parent);
};

#endif
