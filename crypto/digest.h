/*
 * digest.h
 *
 * C++ Classes for Digest algorithms.
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

#ifndef __DIGEST_H__
#define __DIGEST_H__

#include <string>

#include "sha2.h"

namespace Crypto {

    class DigestAlgo {
        protected:
            // Size of digest in bytes.
            int digest_size;
            // Size of a block in bytes.
            int block_size;

        public:

            virtual ~DigestAlgo() {};
            virtual void reset() = 0;
            virtual void update(const u_int8_t *data, const size_t size) = 0;
            virtual void final(u_int8_t digest[]) = 0;

            // Returns size of digest in bytes.
            int size_bytes() const {
                return digest_size;
            }

            // Return size of block in bytes.
            int block_bytes() const {
                return block_size;
            }

            // Adds string data to digest computation.
            friend DigestAlgo& operator << (DigestAlgo& algo, const std::string& s) {
                algo.update((const u_int8_t *)(s.data()), s.size());
                return algo;
            }

            // Computes final hexadecimal of digest and concatenates to a string.
            friend DigestAlgo& operator >> (DigestAlgo& algo, std::string& s);

    };

    class SHA256 : public DigestAlgo {
        private:
            SHA256_CTX ctx;
        public:
            SHA256() {
                digest_size = 256 / 8;
                block_size = 512 / 8;
                reset();
            }
            void reset() {
                SHA256_Init(&ctx);
            }
            void update(const u_int8_t *data, const size_t size) {
                SHA256_Update(&ctx, data, size);
            }
            void final(u_int8_t digest[]) {
                SHA256_Final(digest, &ctx);
            }
    };

    class SHA384 : public DigestAlgo {
        private:
            SHA384_CTX ctx;
        public:
            SHA384() {
                digest_size = 384 / 8;
                block_size = 1024 / 8;
                reset();
            }
            void reset() {
                SHA384_Init(&ctx);
            }
            void update(const u_int8_t *data, const size_t size) {
                SHA384_Update(&ctx, data, size);
            }
            void final(u_int8_t digest[]) {
                SHA384_Final(digest, &ctx);
            }
    };

    class SHA512 : public DigestAlgo {
        private:
            SHA512_CTX ctx;
        public:
            SHA512() {
                digest_size = 512 / 8;
                block_size = 1024 / 8;
                reset();
            }
            void reset() {
                SHA512_Init(&ctx);
            }
            void update(const u_int8_t *data, const size_t size) {
                SHA512_Update(&ctx, data, size);
            }
            void final(u_int8_t digest[]) {
                SHA512_Final(digest, &ctx);
            }
    };

}

#endif
