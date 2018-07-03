#include "malloc.h"
#include "utils.h"

void *memalign(size_t alignment, size_t size) {
  void *ptr = malloc(size + alignment + MALLOC_HEADER_SIZE);
  if (ptr) {
    void *ret = roundUp(ptr, alignment);
    ((MallocHeader *)MALLOC_BASE(ret))->header_addr = ptr;
    return ret;
  } else {
    return NULL;
  }
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
  void *ret = memalign(alignment, size);
  if (ret) {
    *memptr = ret;
    return 0;
  } else {
    return errno;
  }
}