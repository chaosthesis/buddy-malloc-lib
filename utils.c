#include "utils.h"
#include "malloc.h"

MemArena arena;

int log_2(int n) {
  int i = 0;
  while (n >> (i + 1)) i++;
  return i;
}

void *roundUp(void *addr, size_t alignment) {
  return (void *)((((ul)addr + sizeof(ul)) & ~(ul)(alignment - 1)) + alignment);
}

void popFront(int index) {
  MallocHeader *hdr = arena.freeList[index];
  arena.freeList[index] = hdr->next;
  hdr->next = NULL;
  hdr->is_free = 0;
}

void splitBlock(int index) {
  MallocHeader *blockA = arena.freeList[index];
  blockA->size /= 2;
  blockA->is_free = 1;
  popFront(index);

  MallocHeader *blockB = (MallocHeader *)((char *)blockA + blockA->size);
  blockB->size = blockA->size;
  blockB->is_free = 1;

  blockA->next = blockB;
  blockB->next = NULL;
  arena.freeList[index - 1] = blockA;
}

MallocHeader *getBuddy(MallocHeader *block) {
  ul offset = (ul)(((char *)block) - (char *)arena.heap);
  ul mask = (ul)(0x1 << log_2(block->size));
  return (MallocHeader *)((offset ^ mask) + (char *)arena.heap);
}

void pushFront(int index, MallocHeader *block) {
  block->next = arena.freeList[index];
  arena.freeList[index] = block;
  block->is_free = 1;
}

void mergeBlock(MallocHeader *block) {
  int index = log_2(block->size / MIN_BLOCK);
  if (index >= MAX_ORDER) {
    pushFront(index, block);
    return;
  }

  MallocHeader *buddy = getBuddy(block);
  if (!buddy->is_free || buddy->size != block->size) {
    pushFront(index, block);
    return;
  }

  MallocHeader *cur = arena.freeList[index];
  if (cur == buddy) {
    popFront(index);
  } else {
    while (cur->next != buddy) cur = cur->next;
    cur->next = buddy->next;
    buddy->next = NULL;
  }

  if (block < buddy) {
    block->size *= 2;
    mergeBlock(block);
  } else {
    buddy->size *= 2;
    mergeBlock(block);
  }
}
