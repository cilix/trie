#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define KEY_MAX 32

typedef unsigned long int nWord_t;
typedef unsigned char nByte_t;

/* to change trie span, both variables below must be changed */
#define TRIE_SPAN  3 /* trie span */
#define TRIE_LIMIT 8 /* 2 ^ span */

#define TRIE_POOL  256
#define BYTE_SIZE  8

typedef union trieElem TrieElem;
typedef TrieElem Trie;

const int trieVal = TRIE_LIMIT;
const int trieMask = 0xff >> (BYTE_SIZE - TRIE_SPAN);
const int trieOff = BYTE_SIZE / TRIE_SPAN;
const int trieDiff = BYTE_SIZE - TRIE_SPAN;

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

int trieStrlen (nByte_t* str) {
  unsigned int len, nlen;
  len = (unsigned int)strlen((const char *)str);
  nlen = (len > KEY_MAX ? KEY_MAX : len);
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
  unsigned i, l, c, m, n, s;
  l = trieStrlen(key) * trieOff;
  for (i = 0; i < l; i++) {
    m = i % trieOff, n = i / trieOff;
    s = (trieDiff - (m * TRIE_SPAN));
    c = (key[n] & (trieMask << s)) >> s;
    if (!t->sub[c]) {
      if (!alloc) { return NULL; }
      t->sub[c] = (nWord_t)trieElemInit();
    }
    t = (TrieElem *)t->sub[c];
  }
  return t;
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

void teststr(char * str)
{
  int i = 0;
  char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
  for ( ; i < 9; i++) {
    str[i] = alphabet[rand() % 26];
  }
  str[i] = 0;
}

int main (void)
{
  Trie* trie = trieInit();
  int i;
  double end, start;
  char str[10];
  srand(time(NULL));
  start = (float)clock()/CLOCKS_PER_SEC;
  for( i=0; i<100000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, "val");
  }
  trieAdd(trie, (unsigned char*)"hello", "world");
  for( i=0; i<100000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, "val");
  }
  end = (float)clock()/CLOCKS_PER_SEC;
  printf("Used %lu total bytes.\n", bytes);
  printf("Finshed in %f. Press enter.", end-start);
  getchar();
  puts((const char*)trieGet(trie, (unsigned char*)"hello"));
  return 0;
}