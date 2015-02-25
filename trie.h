/* Trie
 *
 * Copyright (c) 2015, Matthew Levenstein
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of Redis nor the names of its contributors may be used
 *      to endorse or promote products derived from this software without
 *      specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TRIE_H
#define _TRIE_H

typedef unsigned long int nWord_t;
typedef unsigned char nByte_t;

/* to change trie span, both variables below must be changed */
#define TRIE_SPAN  5 /* trie span */
#define TRIE_LIMIT 32 /* 2 ^ span */

/* this doesn't need to be changed, but doing so accordingly 
(with the above variables) can greatly impact performance */
#define TRIE_KEY_MAX 9

#define TRIE_POOL  64
#define BYTE_SIZE  8

typedef union trieElem TrieElem;
typedef TrieElem Trie;

size_t bytes;

union trieElem {
  nWord_t sub[TRIE_LIMIT + 1];
};

int trieStrlen (nByte_t *);
void* trieMalloc (nWord_t);
Trie* trieInit (void);
int trieAdd (Trie* t, nByte_t *, void *);
void* trieGet (Trie* t, nByte_t* key);
Trie* trieElemInit (void);
TrieElem* trieFind (Trie *, nByte_t *, int);

#endif