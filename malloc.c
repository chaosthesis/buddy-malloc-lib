#include "malloc.h"
#include "utils.h"

MemArena arena;
pthread_mutex_t malloc_lock;

void *buddy_malloc(size_t allocSize) {
  MallocHeader *hdr;

  int blockSize = MIN_BLOCK;
  int order = 0;

  while (blockSize < allocSize) {
    blockSize *= 2;
    order++;
  }

  pthread_mutex_lock(&malloc_lock);

  if (arena.freeList[order]) {
    hdr = arena.freeList[order];
    popFront(order);
    pthread_mutex_unlock(&malloc_lock);
    return hdr;
  }

  while (++order <= MAX_ORDER) {
    if (arena.freeList[order]) {
      splitBlock(order);
      pthread_mutex_unlock(&malloc_lock);
      return buddy_malloc(allocSize);
    }
  }

  hdr = sbrk(BUDDY_SIZE);
  if (hdr == (void *)-1) {
    return hdr;
  }

  hdr->size = BUDDY_SIZE;
  hdr->next = NULL;
  hdr->is_free = 1;
  arena.freeList[MAX_ORDER] = hdr;
  pthread_mutex_unlock(&malloc_lock);

  return buddy_malloc(allocSize);
}

void *malloc(size_t size) {
  void *ret;
  size_t allocSize = size + MALLOC_HEADER_SIZE;

  if (allocSize <= MALLOC_HEADER_SIZE) {
    return NULL;
  } else if (allocSize <= BUDDY_SIZE) {
    ret = buddy_malloc(allocSize);
    assert(ret != (void *)-1);
  } else if (allocSize > BUDDY_SIZE) {
    ret = mmap(0, allocSize, PROT_READ | PROT_WRITE,
               MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    assert(ret != MAP_FAILED);
  }

#if DEBUG
  // Can't use printf here because printf internally calls `malloc`
  char buf[1024];
  snprintf(buf, 1024, "%s:%d malloc(%zu): Allocated %zu bytes at %p\n",
           __FILE__, __LINE__, size, allocSize, ret);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
#endif

  MallocHeader *hdr = (MallocHeader *)ret;
  hdr->size = allocSize;

  if (arena.heap == NULL) arena.heap = ret;

  return ret + MALLOC_HEADER_SIZE;
}

__attribute__((constructor)) void init() {
  arena.heap = NULL;
  pthread_mutex_init(&malloc_lock, NULL);
}
