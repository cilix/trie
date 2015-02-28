#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "trie.h"

void teststr(char * str)
{
  int i = 0;
  char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
  for ( ; i < 31; i++) {
    str[i] = alphabet[rand() % 26];
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
  char str[32];
  srand(time(NULL));
  start = (float)clock()/CLOCKS_PER_SEC;
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, "val");
  }
  trieAdd(trie, (unsigned char*)"hello", "world");
  for( i=0; i<500000; ++i ){
    teststr(str);
    trieAdd(trie, (unsigned char*)str, "val");
  }
  end = (float)clock()/CLOCKS_PER_SEC;
  printf("Used %lu total bytes.\n", bytes);
  printf("Finshed in %f. Press enter.", end-start);
  getchar();
  puts((const char*)trieGet(trie, (unsigned char*)"hello"));
  trieDelete(trie, (unsigned char *)"hello", &destroy);
  return 0;
}