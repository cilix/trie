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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "trie.h"

const int trieVal = TRIE_LIMIT;
const int trieDiff = BYTE_SIZE - TRIE_SPAN;
const int trieMask = 0xff >> trieDiff;
const int trieOff = BYTE_SIZE / TRIE_SPAN;

#define trieStep() do { \
  m = i % trieOff, n = i / trieOff; \
  s = (trieDiff - (m * TRIE_SPAN)); \
  c = (key[n] & (trieMask << s)) >> s; \
} while (0)

int trieStrlen (nByte_t* str) {
  unsigned int len, nlen;
  len = (unsigned int)strlen((const char *)str);
  nlen = (len > TRIE_KEY_MAX ? TRIE_KEY_MAX : len);
  return (int)nlen;
}

void* trieMalloc (size_t size) {
  void* buf = malloc(size);
  if (!buf) {
    fprintf(stderr, "Fatal: out of memory.");
    abort();
  }
  memset(buf, 0, size);
  bytes += size;
  return buf;
}

Trie* trieElemInit (void) {
  static int tcnt = TRIE_POOL;
  static Trie* pool = NULL;
  Trie* t;
  if (tcnt == TRIE_POOL) {
    pool = NULL;
    pool = trieMalloc(sizeof(Trie) * TRIE_POOL);
    tcnt = 0;
  }
  t = &pool[tcnt++];
  return t;
}

Trie* trieInit (void) {
  return trieElemInit();
}

TrieElem* trieFind (Trie* t, nByte_t* key, int alloc) {
  int i, l, c, m, n, s;
  l = trieStrlen(key) * trieOff;
  for (i = 0; i < l; i++) {
    trieStep();
    if (!t->sub[c]) {
      if (!alloc) { return NULL; }
      t->sub[c] = (nWord_t)trieElemInit();
    }
    t = (TrieElem *)t->sub[c];
  }
  return t;
}

int trieDelete (Trie* t, nByte_t* key, void (*trieValDestroy)(nWord_t)) {
  unsigned i, l, c, m, n, s, u, z;
  nWord_t* stack;
  TrieElem* del;
  z = 0, l = trieStrlen(key) * trieOff;
  stack = trieMalloc(l);
  for (i = 0; i < l; i++) {
    trieStep();
    if (!t->sub[c]) { return 0; }
    stack[z++] = (nWord_t)t;
    t = (TrieElem *)t->sub[c];
  }
  stack[z] = (nWord_t)t;
  u = 0;
  del = (TrieElem *)stack[z--];
  if (del->sub[trieVal]) {
    trieValDestroy(del->sub[trieVal]);
  }
  for (i = 0; i < trieVal; i++) {
    if (del->sub[i]) { u += 1; }
  }
  if (!u) { free((void *)del); }
  while (z >= 0) {
    u = 0;
    del = (TrieElem *)stack[z--];
    for (i = 0; i <= trieVal; i++) {
      if (del->sub[i] == stack[z+1]) {
        del->sub[i] = 0;
      } else if (del->sub[i]) { 
        u += 1;
      }
    }
    if (!u) { free((void *)del); }
  }
  return 1;
}

int trieAdd (Trie* t, nByte_t* key, void* val) {
  t = trieFind(t, key, 1);
  if (!t) { return 0; }
  t->sub[trieVal] = (nWord_t)val;
  return 1;
}

void* trieGet (Trie* t, nByte_t* key) {
  t = trieFind(t, key, 0);
  if (!t) { return NULL; }
  return (void *)t->sub[trieVal];
}
