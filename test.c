#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash.h"

Byte_t* getWord (FILE* f) {
  Byte_t* buf = NULL;
  int c = 0, i = 0, bufsize = 10;
  buf = malloc(bufsize + 1);
  memset(buf, 0, bufsize + 1);
  while ((c = fgetc(f)) != '\n') {
    if (c == EOF) return NULL;
    if (i == bufsize)
      buf = realloc(buf, (bufsize += 10) + 1);
    buf[i++] = (Byte_t)c;
  }
  buf[i] = 0;
  return buf;
} 

void loadWords (HashTable* root) {
  FILE* in = fopen("words.txt", "r");
  Byte_t* word = NULL;
  while ((word = getWord(in))) {
    int l = strlen((char *)word);
    hashset(root, word, l, (Word_t)word);
  }
  fclose(in);
  /**
  puts("inserted");
  getchar();
  in = fopen("words.txt", "r");
  while ((word = getWord(in))) {
    int l = strlen((char *)word);
    hashdelete(root, word, l);
  } 
  /**/
}

int main( ){
  
  double end, start;
  HashTable* h = hashinit();

  start = (float)clock()/CLOCKS_PER_SEC;
  hashset(h, (Byte_t *)"matthew", 7, (Word_t)"levenstein");
  loadWords(h);
  hashset(h, (Byte_t *)"matthew", 7, (Word_t)"barfenbutt");
  end = (float)clock()/CLOCKS_PER_SEC;

  printf("Inserted %d objects in %lfs - Table size: %d\n", h->c, end - start, h->s);
  return 0;
}
