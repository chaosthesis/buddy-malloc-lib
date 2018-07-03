#include "malloc.h"
#include "utils.h"

MemArena arena;
pthread_mutex_t malloc_lock;

void free(void *ptr) {
  if (ptr == NULL) return;

  MallocHeader *hdr = MALLOC_BASE(ptr);
  if (hdr->header_addr) return free(hdr->header_addr);

  if (hdr->size <= BUDDY_SIZE) {
    if (hdr->is_free == 0) {
      hdr->is_free = 1;
      hdr->next = NULL;
      pthread_mutex_lock(&malloc_lock);
      mergeBlock(hdr);
      pthread_mutex_unlock(&malloc_lock);
    }
  } else if (hdr->size > BUDDY_SIZE) {
    munmap(hdr, hdr->size);
  }

#if DEBUG
  // Can't use printf here because printf internally calls `malloc`
  char buf[1024];
  snprintf(buf, 1024, "%s:%d free(%p): Freeing %zu bytes from %p\n", __FILE__,
           __LINE__, ptr, hdr->size, hdr);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
#endif
}
