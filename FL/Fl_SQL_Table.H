/*
 * Fl_SQL_Table.H
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

#ifndef __FL_SQL_TABLE_H__
#define __FL_SQL_TABLE_H__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "FL/Fl_Table_Row.H"
#include "sqlite/sql.h"

class Fl_SQL_Table : public Fl_Table_Row {
    private:
        char ***cache;
        int cache_rowstart, cache_rowlen;

        int hidden_cols;
        Fl_Color sel_text_color;
    protected:
        // Implement with a function that returns the displayed name of a column.
        virtual const char *get_colname(int i) = 0;
        // Implement with a function that queries table for rowlen rows starting at rowstart.
        virtual void query_data(int rowstart, int rowlen, sqlite3_callback cb, void *cbarg) = 0;
        // Implement with a function that queries table for the number of rows.
        virtual void query_numrows(sqlite3_callback cb, void *cbarg) = 0;

        void draw_cell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
    public:
        Fl_SQL_Table(int x, int y, int w, int h, int num_hidden_cols = 0, const char *l=0);
        ~Fl_SQL_Table();
        // Initialize dimensions. Number of rows will be queried from virtual function.
        // It's because virtual functions can't be called from constructor.
        void init_dimensions(int numcols);
        // Clear the visible area cache.
        void clear_cache();
        // Get cell value from visible area cache.
        const char *get_cell_value(int row, int col);
        // Notify difference in number of rows.
        void rows_changed(int diff) {
            rows(rows() + diff);
            clear_cache();
            select_all_rows(0);
        }
        // Get and set selection text color.
        Fl_Color selection_text_color() {
            return sel_text_color;
        }
        void selection_text_color(Fl_Color color) {
            sel_text_color = color;
        }
};

#endif
