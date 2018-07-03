# A Malloc Library Implementation 

## Design overview

   The objective of HW3 is to implement a malloc library, which includes `malloc`,
   `free`, `realloc`, `calloc`, `memalign` and other dynamic memory allocation
   routines. In this particular implementation, it is required to use **buddy
   algorithm** for memory allocation. In addtion, the returned address from this
   library should be aligned at 8-byte boundary, and the library should be thread-
   safe. More details can be found at:
   http://www.ccs.neu.edu/home/kapil/courses/cs5600sp18/hw3.html
   
## Design decisions
   
- sbrk() or mmap():
  `malloc` choose between two mechanisms to request more memory from the kernel:
  it evokes sbrk() (i.e. buddy memory allocstion) if the requested memory is
  smaller than 2048 bytes, and evokes mmap() otherwise;
 
- Memory arena data structure:
  The `MemArena` struct maintains information of memory arena. It consists of
  a heap pointer (base memory address for sbrk()) and a `freeList[]` which keeps
  track of all free blocks;
   
- Memory block data structure:
  The `MallocHeader` struct maintains meta information of a memory block,
  including its size, in-use status, next same-size free block, etc. A
  `MallocHeader` variable is normally in front of the allocated memory
  address returned to the user;
 
- `freeList` data structure:
  It is an array of pointers of `MallocHeader`. A free buddy block of a
  particular order will be assigned to the array element whose index equals
  the order. Essentially, each element is a header of a linked list of free
  blocks of the same order;
  
- malloc(), free(), and the like:
  When processing memory blocks that are smaller than 2048 bytes, the library
  actively updates the `freeList` array in the arena. To allocate a memory
  block, the `freeList` pop out a suitable block; to free a memory block, the
  block merges with its buddy block when possible, and is pushed into the
  `freeList`;
   
- memalign():
  To meet the alignment requirement, the library calls malloc() to allocate
  a memory block larger than requested so an aligned address can be found
  in the middle.
     
For detailed design decisions please refer to the code.
