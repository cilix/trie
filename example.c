#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "trie.h"

#define KEY_LENGTH 15

void teststr(nByte_t * str)
{
  int i = 0;
  for ( ; i < KEY_LENGTH; i++) {
    str[i] = (rand() % 256);
  }
  str[i] = 0;
}

void destroy (nWord_t val) {
  /* custom destroy function here for node values */
}

int main (void)
{
  Trie* trie = trieInit();
  int i;
  double end, start;
  nByte_t str[32];
  srand(time(NULL));
  start = (float)clock()/CLOCKS_PER_SEC;
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, KEY_LENGTH, "val");
  }
  trieAdd(trie, (unsigned char*)"hello", 5, "world");
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, KEY_LENGTH, "val");
  }
  end = (float)clock()/CLOCKS_PER_SEC;
  printf("Used %lu total bytes.\n", bytes);
  printf("Finshed in %f. Press enter.", end-start);
  getchar();

  puts((const char*)trieGet(trie, (unsigned char*)"hello", 5));
  trieDelete(trie, (unsigned char *)"hello", 5, &destroy);
  return 0;
}
