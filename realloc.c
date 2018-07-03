#include "malloc.h"
#include "utils.h"

void *realloc(void *ptr, size_t size) {
  if (ptr == NULL) return malloc(size);

  MallocHeader *hdr = MALLOC_BASE(ptr);
  if (hdr->size >= size) return ptr;

  void *ret = malloc(size);
  memcpy((char *)ret, ptr, hdr->size);
  free(ptr);

  return ret;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  return realloc(ptr, nmemb * size);
}
