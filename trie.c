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
  Trie* t = trieMalloc(sizeof(Trie));
  return t;
}

Trie* trieInit (void) {
  return trieElemInit();
}

TrieElem* trieFind (Trie* t, nByte_t* key, int len, int alloc) {
  int i, l, m, n, s;
  unsigned c;
  l = len * trieOff;
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

int trieDelete (Trie* t, nByte_t* key, int len, void (*destroy)(nWord_t)) {
  int i, l, m, n, s, u, z;
  unsigned c;
  nWord_t* stack;
  TrieElem *d, *h;
  z = 0, l = len * trieOff;
  stack = trieMalloc(l);
  for (i = 0; i < l; i++) {
    trieStep();
    if (!t->sub[c]) { return 0; }
    stack[z++] = (nWord_t)t;
    t = (TrieElem *)t->sub[c];
  }
  stack[z] = (nWord_t)t;
  u = 0;
  d = (TrieElem *)stack[z--];
  if (d->sub[trieVal]) {
    destroy(d->sub[trieVal]);
    d->sub[trieVal] = 0;
  }
  for (i = 0; i < trieVal; i++) {
    if (d->sub[i]) { u += 1; }
  }
  if (!u) { free(d); }
  while (z >= 0) {
    u = 0;
    d = (TrieElem *)stack[z--];
    for (i = 0; i <= trieVal; i++) {
      if (d->sub[i] == stack[z+1]) {
        d->sub[i] = 0;
      } else if (d->sub[i]) { 
        u += 1;
      }
    }
    if (!u) { free((void *)d); }
  }
  return 1;
}

int trieAdd (Trie* t, nByte_t* key, int len, void* val) {
  t = trieFind(t, key, len, 1);
  if (!t) { return 0; }
  t->sub[trieVal] = (nWord_t)val;
  return 1;
}

void* trieGet (Trie* t, nByte_t* key, int len) {
  t = trieFind(t, key, len, 0);
  if (!t) { return NULL; }
  return (void *)t->sub[trieVal];
}
