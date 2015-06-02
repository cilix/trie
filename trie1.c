#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned long int word_t;
typedef unsigned char     byte_t;

typedef struct trieElem trieElem;
typedef trieElem        trie;

size_t bytes; /* cuz memory */

struct trieElem {
  byte_t  head[4];
  byte_t* pref; /* path compression */
  word_t* radix;
  word_t  val;
}; 

/* general */
void* trieMalloc (size_t);
void  trieFree   (void*);

/* dict api */
trieElem* trieElemAlloc  (void);
trie*     trieAlloc      (void);
void           trieElemResize (trieElem*, int);
trieElem* trieFind       (trie*, byte_t*, int, int);
int            trieAdd        (trie*, byte_t*, int, void*);
void*          trieGet        (trie*, byte_t*, int);
void           triePrintNode  (trieElem*);

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

void trieFree (void* obj) {
  bytes -= sizeof(*obj);
  free(obj);
}

/* first 4 bytes
 *    byte 1: type (1, 2, 4, 8, 0(for 16))
 *    byte 2: count (0 - 15)
 *    byte 3 + 4: child map (16 bits noting the slot)
 */
trieElem* trieElemAlloc (void) {
  trieElem* e = trieMalloc(sizeof(trieElem));
  e->radix = trieMalloc(sizeof(word_t));
  e->head[0] = 1;
  e->head[1] = 0;
  e->head[2] = e->head[3] = 0;
  return e;
}

trie* trieAlloc (void) {
  return trieElemAlloc();
}

void trieElemResize (trieElem* e, int aord) {
  int i = 0, j = 0;
  byte_t l = e->head[1];
  byte_t type = e->head[0];
  byte_t size = l * (2 * aord);
  word_t* radix = e->radix;
  e->radix = trieMalloc(sizeof(word_t) * size);
  memset(e->radix, 0, size);
  for (; i < type; i++, j++) {
    if (radix[i]) e->radix[j] = radix[i];
  }
  e->head[0] = size;
  trieFree(radix);
  return;
}

void triePrintNode (trieElem* e) {
  int i = 0;
  int l = e->head[1];
  printf("NODE(0x%lx)\n", (word_t)e);
  for (; i < l; i++)
    printf("Index %d - 0x%lx\n", i, (word_t)e->radix[i]);
  printf("Bits flipped: |");
  for (i = 0; i < 16; i++) {
    int v = 0x80 >> (i % 8);
    int idx = (i < 8 ? 2 : 3);
    if ((e->head[idx] & v) == v)
      printf(" %d |", i);
  }
  printf("\n");
}

trieElem* trieFind (trie* d, byte_t* key, int len, int alloc) {
  int i, j, l, m, n, s, x, v, f, ct, idx, le;
  unsigned c;
  l = len * 2;
  for (i = 0; i < l; i++) {
    /* break each byte in half */
    m = i % 2, n = i / 2;
    s = (4 - (m * 4));
    c = (key[n] & ((0xff >> 4) << s)) >> s;
    ct = f = 0;
    /* check the bitmap to see if the child (c) exists */
    for (j = 0; j <= c; j++) {
      v = 0x80 >> (j % 8);
      idx = (j < 8 ? 2 : 3);
      if ((d->head[idx] & v) != v)
        continue;
      ct++;
      if (c == j) {
        f = 1;
        break;
      }
    }
    /* if it exists */
    if (f) {
      d = (trieElem *)d->radix[ct - 1];
      continue;
    }
    /* otherwise: */
    if (!alloc) return NULL;
    /* determine which byte of the bitmap to use */
    idx = (c < 8 ? 2 : 3);
    /* flip the bit */
    d->head[idx] |= (0x80 >> (c % 8));

    /* add the child in sorted order */
    le = ++(d->head[1]);
    for (j = le - 1; j > ct; j--)
      d->radix[j] = d->radix[j - 1];
    d->radix[ct] = (word_t)trieElemAlloc();

    /* resize if we've reached the threshold */
    if (le == 1 || (!(le % 2) && le != 6 && le < 16))
      trieElemResize(d, 1);

    d = (trieElem *)d->radix[ct];
  }
  return d;
}

int trieAdd (trie* d, byte_t* key, int len, void* val) {
  d = trieFind(d, key, len, 1);
  if (!d) return 0;
  d->val = (word_t)val;
  return 1;
}

void* trieGet (trie* d, byte_t* key, int len) {
  d = trieFind(d, key, len, 0);
  if (!d) return NULL;
  return (void *)d->val;
}

/***********************************************************************/
/***************************** TESTS ***********************************/
/***********************************************************************/

#define KEY_LEN 15

void teststr(byte_t * str) {
  int i = 0;
  for ( ; i < KEY_LEN; i++) {
    str[i] = (rand() % 256);
  }
  str[i] = 0;
}

unsigned testnum () {
  static unsigned n = 0;
  return ++n;
}

void benchmarkInt () {
  trie* d = trieAlloc();
  int i;
  double end, start;
  unsigned testn;
  srand(time(NULL));
  start = (float)clock()/CLOCKS_PER_SEC;
  for( i=0; i<500000; ++i ){
    testn = testnum();
    trieAdd(d, (unsigned char*)&testn, sizeof(unsigned), "");
  }
  for( i=0; i<500000; ++i ){
    testn = testnum();
    trieAdd(d, (unsigned char*)&testn, sizeof(unsigned), "");
  }
  end = (float)clock()/CLOCKS_PER_SEC;
  printf("Used %lu total bytes.\n", bytes);
  printf("Finshed in %f.\n", end-start);
}

void benchmarkStr () {
  trie* d = trieAlloc();
  int i;
  double end, start;
  byte_t str[KEY_LEN + 1];
  srand(time(NULL));
  start = (float)clock()/CLOCKS_PER_SEC;
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(d, (unsigned char*)str, KEY_LEN, "");
  }
  trieAdd(d, (unsigned char*)"hello", 5, "world");
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(d, (unsigned char*)str, KEY_LEN, "");
  }
  end = (float)clock()/CLOCKS_PER_SEC;
  printf("Used %lu total bytes.\n", bytes);
  printf("Finshed in %f. Press enter.", end-start);
  getchar();

  puts((const char*)trieGet(d, (unsigned char*)"hello", 5));
}

int main (void) {
  benchmarkStr();
  return 0;
}