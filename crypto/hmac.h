/*
 * hmac.h
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

#ifndef __HMAC_H__
#define __HMAC_H__

#include "digest.h"

#include <string>

namespace Crypto {

    class HMAC : public DigestAlgo {
        private:
            DigestAlgo& algo;
            u_int8_t *key;

            void init(const u_int8_t hmac_key[], const int hmac_key_len);

        public:
            HMAC(DigestAlgo &dig_algo, const u_int8_t hmac_key[], const int hmac_key_len) :algo(dig_algo) {
                init(hmac_key, hmac_key_len);
            }
            HMAC(DigestAlgo &dig_algo, const std::string &hmac_key) :algo(dig_algo) {
                init((const u_int8_t *)hmac_key.data(), hmac_key.size());
            }

            ~HMAC() {
                delete [] key;
            }

            void reset();
            void final(u_int8_t digest[]);

            void update(const u_int8_t *data, size_t size) {
                algo.update(data, size);
            }
    };

}

#endif
