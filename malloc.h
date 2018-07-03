#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <stdlib.h>

//#define PAGE_SIZE (sysconf(_SC_PAGESIZE))
#define MIN_BLOCK 8
#define MAX_ORDER 8
#define BUDDY_SIZE 2048

typedef struct MallocHeader {
  size_t size;
  struct MallocHeader *next;
  int is_free;
  void* header_addr;
} MallocHeader;

typedef struct MemArena {
  void *heap;
  struct MallocHeader *freeList[MAX_ORDER + 1];
  /* order: 0, 1,  2,  ..., 8
     block: 8, 16, 32, ..., 2048 */
} MemArena;

extern MemArena arena;
extern pthread_mutex_t malloc_lock;

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocarray(void *ptr, size_t nmemb, size_t size);
void *memalign(size_t alignment, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size);

#define MALLOC_HEADER_SIZE (sizeof(MallocHeader))
#define MALLOC_BASE(addr) ((void *)((char *)addr - MALLOC_HEADER_SIZE))

#endif