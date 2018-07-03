#include "malloc.h"
#include "utils.h"

void* calloc(size_t nmemb, size_t size) {
  int totalSize = nmemb * size;
  
  void* ret = malloc(totalSize);
  if (ret) memset(ret, 0, totalSize);
  return ret;
}
