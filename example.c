#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "trie.h"

#define KEY_LENGTH 9

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
  puts((const char *)val);
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
  trieAdd(trie, (unsigned char*)"hell", 4, "is a wonderful place");
  trieAdd(trie, (unsigned char*)"he'll", 5, "never marry you");
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, KEY_LENGTH, "val");
  }
  end = (float)clock()/CLOCKS_PER_SEC;
  printf("Used %lu total bytes.\n", bytes);
  printf("Finshed in %f. Press enter.", end-start);
  getchar();

  puts((const char*)trieGet(trie, (unsigned char*)"hello", 5));
  return 0;
}
