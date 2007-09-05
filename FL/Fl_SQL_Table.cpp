/*
 * Fl_SQL_Table.cpp
 *
 * Tabela para consultas SQL.
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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <cstdlib>
#include <cstring>

#include "sqlite/sql.h"
#include "FL/Fl_SQL_Table.H"


Fl_SQL_Table::Fl_SQL_Table(int x, int y, int w, int h, int num_hidden_cols, const char *l) :Fl_Table_Row(x,y,w,h,l) {
    type(SELECT_SINGLE);
    selection_color(FL_DARK_BLUE);
    selection_text_color(FL_WHITE);

    cache = 0;
    hidden_cols = num_hidden_cols;

    col_header(1);
    col_resize(4);
    row_header(0);
    row_resize(0);

    end();
}

Fl_SQL_Table::~Fl_SQL_Table() {
    clear_cache();
}

void Fl_SQL_Table::clear_cache() {
    if(cache) {
        for(int i = 0; i < cache_rowlen; i++) {
            for(int j = 0; j < hidden_cols + cols(); j++) {
                free(cache[i][j]);
            }
            delete [] cache[i];
        }
        delete [] cache;
        cache = 0;
    }
}

struct query_data_data {
    char ***cache;
    int cols;
    int rows;
    int row_idx;
};

static int query_data_cb(void *arg, int argc, char **argv, char **colname) {
    query_data_data &cb_data = *(query_data_data *)arg;

    if(cb_data.row_idx < cb_data.rows) {
        int cols = cb_data.cols;
        char **cache = cb_data.cache[cb_data.row_idx] = new char*[cols];

        for(int i = 0; i < cols; i++) {
            cache[i] = strdup(((i < argc) && (argv[i])) ? argv[i] : "");
        }

        cb_data.row_idx++;
    }

    return 0;
}

const char *Fl_SQL_Table::get_cell_value(int row, int col) {
    if(!(cache && (row >= cache_rowstart) && (row < (cache_rowstart + cache_rowlen)))) {
        clear_cache();

        cache_rowlen = 2 * (botrow - toprow + 1);
        cache_rowstart = row - (botrow - toprow + 1) / 2;

        if(cache_rowstart < 0) {
            cache_rowstart = 0;
        }

        if(cache_rowstart + cache_rowlen > rows()) {
            cache_rowlen = rows() - cache_rowstart;
        }

        cache = new char**[cache_rowlen];

        query_data_data cb_data = { cache, hidden_cols + cols(), cache_rowlen, 0 };
        query_data(cache_rowstart, cache_rowlen, query_data_cb, &cb_data);
    }

    return cache[row - cache_rowstart][col];
}

static int query_numrows_cb(void *arg, int argc, char **argv, char **colname) {
    int *numrows = (int *)arg;
    *numrows = (argc == 1) ? atoi(argv[0]) : 0;
    return 0;
}

void Fl_SQL_Table::init_dimensions(int numcols) {
    int numrows = 0;
    query_numrows(query_numrows_cb, &numrows);

    cols(numcols);
    rows(numrows);
}

void Fl_SQL_Table::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
    switch(context) {
        case CONTEXT_STARTPAGE:
            fl_font(FL_HELVETICA, 16);
            return;
        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
                fl_color(FL_BLACK);
                fl_draw(get_colname(C), X, Y, W, H, FL_ALIGN_CENTER);
            }
            fl_pop_clip();
            return;
        case CONTEXT_CELL:
            fl_push_clip(X, Y, W, H);
            {
                // BG COLOR
                fl_color( row_selected(R) ? selection_color() : selection_text_color());
                fl_rectf(X, Y, W, H);

                // TEXT
                fl_color( row_selected(R) ? FL_WHITE : FL_BLACK);
                fl_draw(get_cell_value(R, hidden_cols + C), X, Y, W, H, FL_ALIGN_LEFT, 0, 0);

                // BORDER
                fl_color(FL_LIGHT2);
                fl_rect(X, Y, W, H);
            }
            fl_pop_clip();
            return;
        default:
            return;
    }
}

