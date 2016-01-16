#ifndef _HASH_H
#define _HASH_H

#define Word_t unsigned long
#define Byte_t unsigned char

#define _H_ALLOC  256
#define _H_WS     sizeof(Word_t)

struct __hn {
  Byte_t k[_H_WS]; /* key */
  Word_t v; /* value */
  int    l; /* key length */
  Byte_t f; /* flags (occupied, hash function) */
};

struct __ht {
  struct __hn* t; /* table */
  int          c; /* count */
  int          s; /* size */
};

typedef struct __ht HashTable;

void         hashw2b( Byte_t*, Word_t );
Word_t       hashb2w( Byte_t* );
struct __ht* hashinit( void );
struct __ht* hashalloc( int );
void         hashnodeinit( struct __hn*, Byte_t*, int, Word_t, int );
int          hashmatch( struct __hn*, Byte_t*, int );
void         hashset( struct __ht*, Byte_t*, int, Word_t );
Word_t       hashget( struct __ht*, Byte_t*, int );
void         hashresize( struct __ht*, int );
int          hashfind( struct __ht*, Byte_t*, int, int );
void         hashdelete( struct __ht*, Byte_t*, int );
void         hashnodeclear( struct __hn* );
Byte_t*      hashnodekey( struct __hn* );
struct __hn* hashnodeclone( struct __hn* );

#endif
