/*
 * hmac.cpp
 *
 * C++ Class for HMAC Algorithm.
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

#include <cstring>
#include <cstdio>

#include "digest.h"
#include "hmac.h"

namespace Crypto {

    void HMAC::init(const u_int8_t hmac_key[], const int hmac_key_len) {

        digest_size = algo.size_bytes();
        block_size  = algo.block_bytes();

        key = new u_int8_t[block_size];
        memset(key, 0, block_size);

        if(hmac_key_len <= block_size) {
            memcpy(key, hmac_key, hmac_key_len);
        }
        else {
            algo.update(hmac_key, hmac_key_len);
            algo.final(key);
        }

        reset();

    }

    void HMAC::reset() {

        u_int8_t ipad[block_size];

        for(int i = 0; i < block_size; i++) {
            ipad[i] = key[i] ^ 0x36;
        }

        algo.reset();
        algo.update(ipad, block_size);

    }

    void HMAC::final(u_int8_t digest[]) {

        u_int8_t hash[digest_size];
        u_int8_t opad[block_size];

        algo.final(hash);
        algo.reset();

        for(int i = 0; i < block_size; i++) {
            opad[i] = key[i] ^ 0x5c;
        }

        algo.update(opad, block_size);
        algo.update(hash, digest_size);
        algo.final(digest);

    }

}
