/*
 * sql.cpp
 *
 * SQLite C++ Wrapper.
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

#include <cstdarg>

#include "sqlite3.h"
#include "sql.h"

SQL::SQL(const char *filename) {
    if(sqlite3_open(filename, &db) != SQLITE_OK) {
        throw(sqlite3_errmsg(db));
    }
}

SQL::~SQL() {
    sqlite3_close(db);
}

void SQL::Exec(char *fmt, sqlite3_callback cb, void *cbarg, ...) {
    va_list ap;
    char *sql;
    int res;

    va_start(ap, cbarg);
    sql = sqlite3_vmprintf(fmt, ap);
    res = sqlite3_exec(db, sql, cb, cbarg, 0);
    sqlite3_free(sql);
    va_end(ap);

    if(res != SQLITE_OK) {
        throw(sqlite3_errmsg(db));
    }

}
