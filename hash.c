/*
 * cuckoo hash
 * author - Matthew Levenstein
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

#define hashfn(n, s, l, z) hash##n(s, l, z)

#define isoccupied(x) (x & 0x1)
#define hashused(x)   ((x >> 1) & 0x1)

#define hash0(s, l, z) do { \
  int i; \
  z = 0; \
  for (i = 0; i < l; i++) \
    z = ((z << 5) + z) + s[i]; \
} while( 0 )

#define hash1(s, l, z) do { \
  int i; \
  z = 2166136261; \
  for (i = 0; i < l; i++) \
    z = (z * 16777619) ^ s[i]; \
} while( 0 )

void hashw2b( Byte_t* b, Word_t w ){
  int idx, i, s = (_H_WS * _H_WS);
  for (i = _H_WS; i <= s; i += _H_WS) {
    idx = (i/_H_WS) - 1;
    b[idx] = (w >> (s - i)) & 0xff;
  }
}

Word_t hashb2w( Byte_t* b ){
  int idx, i, s = (_H_WS * _H_WS);
  Word_t w = 0, n;
  for (i = _H_WS; i <= s; i += _H_WS) {
    idx = (i/_H_WS) - 1;
    n = (Word_t)b[idx];
    w |= (n << (s - i));
  }
  return w;
}

struct __ht* hashinit( void ){
  return hashalloc(_H_ALLOC);
}

struct __ht* hashalloc( int size ){
  struct __ht *h = malloc(sizeof(struct __ht));
  int s = sizeof(struct __hn) * size;
  if( !h ) return NULL;
  h->t = malloc(s);
  h->c = 0;
  h->s = size;
  if( !h->t ){
    free(h);
    return NULL;
  }
  memset(h->t, 0, s);
  return h;
}

void hashnodeclear( struct __hn* n ){
  if( n->l > _H_WS ){
    Byte_t* k = (Byte_t *)hashb2w(n->k);
    free(k);
  }
  memset(n, 0, sizeof(struct __hn));
}

void hashnodeinit( struct __hn* n, Byte_t* k, int l, Word_t v, int h ){
  memset(n, 0, sizeof(struct __hn));
  if( l > _H_WS ){
    Byte_t* s = malloc(l);
    if( !s ) return;
    memcpy(s, k, l);
    hashw2b(n->k, (Word_t)s);
  } else memcpy(n->k, k, l);
  n->l = l;
  n->v = v;
  n->f |= 1;
  n->f |= (h << 1);
}

int hashmatch( struct __hn* n, Byte_t* s, int l ){
  Byte_t* k;
  if( !n->f || n->l != l ) return 0;
  k = hashnodekey(n);
  return !memcmp(k, s, l) || 0;
}

void hashresize( struct __ht* h, int d ){
  int i = 0, os; /* iterator, original table size */
  int s = (h->s * (d ? 2 : 0.5)); /* new table size */
  int c = h->c; /* cache old count */
  int ms = s * sizeof(struct __hn); /* malloc size */
  struct __hn *n, *t, *tmp;
  Byte_t* k;
  t = malloc(ms);
  if( !t ) return;
  memset(t, 0, ms);
  os = h->s; /* store old size */
  tmp = h->t; /* store old table */
  h->t = t; /* set new table */
  h->c = 0; /* erase count to avoid nested resizing */
  h->s = s; /* set new size */
  for( ; i < os ; i++ ){ /* for each node in old table */
    n = &(tmp[i]); /* get node */
    if( !n->f ) continue; /* if it's not filled, ignore */
    if( n->l > _H_WS ){
      k = (Byte_t *)hashb2w(n->k);
    } else k = n->k;
    hashset(h, k, n->l, n->v); /* add node */
  }
  h->c = c;
  free(tmp); /* get rid of old table */
}

Byte_t* hashnodekey( struct __hn* n ){
  Byte_t* k;
  if( n->l > _H_WS ) k = (Byte_t *)hashb2w(n->k);
  else k = n->k;
  return k;
}

struct __hn* hashnodeclone( struct __hn* n ){
  struct __hn* n0 = malloc(sizeof(struct __hn));
  if( !n0 ) return NULL;
  hashnodeinit(n0, hashnodekey(n), n->l, n->v, hashused(n->f));
  return n0;
}

void hashset( struct __ht* h, Byte_t* k, int l, Word_t v ){
  int s = h->s, f = 0;
  unsigned hv = 0, hv0 = 0;
  struct __hn *n, *n0, *del = NULL;
  hashfn(0, k, l, hv0);
  hv0 %= s;
  hv = hashfind(h, k, l, 1);
  n = &(h->t[hv]);
  if( !isoccupied(n->f) ){
    (h->c)++;
    hashnodeinit(n, k, l, v, hv != hv0);
    return;
  }
  if( hashmatch(n, k, l) ) return;
  f = 0;
  while( isoccupied(n->f) ){
    n0 = hashnodeclone(n); /* evict */
    hashnodeinit(n, k, l, v, !f); /* set new node */
    f = hashused(n0->f);
    if( del ) free(del);
    if( !f ) hashfn(1, hashnodekey(n0), n0->l, hv);
    else     hashfn(0, hashnodekey(n0), n0->l, hv);
    hv %= s; /* get new slot */
    n = &(h->t[hv]);
    k = hashnodekey(n0);
    l = n0->l;
    v = n0->v;
    del = n0;
  }
  hashnodeinit(n, k, l, v, !f);
  (h->c)++;
  if( h->c > h->s / 2.5 ) hashresize(h, 1);
}

Word_t hashget( struct __ht* h, Byte_t* k, int l ){
  int n = hashfind(h, k, l, 0);
  if( n > -1 ) return h->t[n].v;
  return 0;
}

int hashfind( struct __ht* h, Byte_t* k, int l, int r ){
  /* if r is 1, return the next occupiable slot */
  /* otherwise just return the index if the key matches */
  struct __hn* n;
  unsigned hv, i = 0;
  for( ; i < 2; i++ ){
    if( !i ) hashfn(0, k, l, hv);
    else     hashfn(1, k, l, hv);
    hv %= h->s;
    n = &(h->t[hv]);
    if( hashmatch(n, k, l) ) return hv;
    if( r && !isoccupied(n->f) ) return hv;
  }
  if( r ) return hv;
  return -1;
}

void hashdelete( struct __ht* h, Byte_t* k, int l ){
  int n = hashfind(h, k, l, 0);
  if( n == -1 ) return;
  hashnodeclear(&(h->t[n]));
  if( --(h->c) < h->s / 5 ) hashresize(h, 0);
}
