/*
 * Teserakt AG LIBE4 C Library
 *
 * Copyright 2018-2020 Teserakt AG, Lausanne, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define E4_SHA3_INTERNAL
#include "e4/crypto/sha3.h"
#include "string.h"

#ifndef __AVR__
sha3_ctx_t common_sha3_ctx;
#endif

/* Initialize the context for SHA3 */

int sha3_init(sha3_ctx_t *c, int mdlen)
{
    memset(c->b, 0, 200);
    c->mdlen = mdlen;
    c->rsiz = 200 - 2 * mdlen;
    c->pt = 0;

    return 0;
}

/* update state with more data */

int sha3_update(sha3_ctx_t *c, const void *data, size_t len)
{
    size_t i;
    int j;

    j = c->pt;
    for (i = 0; i < len; i++)
    {
        c->b[j++] ^= ((const uint8_t *)data)[i];
        if (j >= c->rsiz)
        {
            keccak_f1600(c->b, KECCAKF_ROUNDS);
            j = 0;
        }
    }
    c->pt = j;

    return 0;
}

/* finalize and output a hash */

int sha3_final(void *md, sha3_ctx_t *c)
{
    int i;

    c->b[c->pt] ^= 0x06;
    c->b[c->rsiz - 1] ^= 0x80;
    keccak_f1600(c->b, KECCAKF_ROUNDS);

    for (i = 0; i < c->mdlen; i++)
    {
        ((uint8_t *)md)[i] = c->b[i];
    }

    return 0;
}


/* compute a SHA-3 hash (md) of given byte length from "in" */

void *sha3(const void *in, size_t inlen, void *md, int mdlen)
{
    sha3_init(&common_sha3_ctx, mdlen);
    sha3_update(&common_sha3_ctx, in, inlen);
    sha3_final(md, &common_sha3_ctx);

    return md;
}

void sha3_256_trunc(char* output, const size_t outputlen, const char* input, const size_t inputlen) {
    char hash[SHA3_256_DIGEST_LEN];
    sha3(input, inputlen, hash, SHA3_256_DIGEST_LEN);
    memcpy(output, hash, outputlen);
}

/* SHAKE128 and SHAKE256 extensible-output functionality */

void shake_xof(sha3_ctx_t *c)
{
    c->b[c->pt] ^= 0x1F;
    c->b[c->rsiz - 1] ^= 0x80;
    keccak_f1600(c->b, KECCAKF_ROUNDS);
    c->pt = 0;
}

void shake_out(sha3_ctx_t *c, void *out, size_t len)
{
    size_t i;
    int j;

    j = c->pt;
    for (i = 0; i < len; i++)
    {
        if (j >= c->rsiz)
        {
            keccak_f1600(c->b, KECCAKF_ROUNDS);
            j = 0;
        }
        ((uint8_t *)out)[i] = c->b[j++];
    }
    c->pt = j;
}
