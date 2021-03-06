#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "alloc_map.h"

#define Byte_index(siz,idx) (siz / idx)
#define Bit_index(siz,idx) (siz % idx)
#define On(a) 1UL << (a)
#define Off(a) ~(1UL << (a))


struct alloc_map 
{
  void * start_addr;
  size_t word_size;
  size_t map_size;
  uint8_t bits[];
};


size_t
alloc_map_mem_size_needed(size_t word_size, size_t block_size)
{
  return ((block_size/word_size)+2*sizeof(size_t)+sizeof(void*)+1);
}


void 
alloc_map_create(alloc_map_t *alloc_map, void *start_addr, size_t word_size, size_t block_size)
{
  alloc_map->start_addr = start_addr;
  alloc_map->word_size = word_size;
  alloc_map->map_size = (block_size/word_size);
  for(size_t i = 0; i <= (alloc_map->map_size); ++i)
    {
      alloc_map->bits[i] = 0;
    }
}


static size_t 
alloc_map_check_offset(alloc_map_t *alloc_map, void *ptr){
  size_t mem_offset = (size_t)ptr - (size_t)alloc_map->start_addr;
  if(((mem_offset % (alloc_map->word_size)) == 0) && 
     (alloc_map->map_size > (mem_offset/alloc_map->word_size)) &&
     ((size_t)ptr >= (size_t)alloc_map->start_addr))
    {
      return mem_offset;
    }
  else 
    {
      return -1;
    }
}
 

bool 
alloc_map_ptr_used(alloc_map_t *alloc_map, void *ptr)
{
  size_t mem_offset = alloc_map_check_offset(alloc_map, ptr);
  if(mem_offset == (size_t)-1)
    {
      return false;
    }
  return alloc_map->bits[mem_offset / alloc_map->word_size] & On(mem_offset % alloc_map->word_size);
}


bool
alloc_map_set(alloc_map_t *alloc_map, void *ptr, bool state)
{
  size_t mem_offset = alloc_map_check_offset(alloc_map, ptr);
  if(mem_offset == (size_t)-1)
    {
      assert(false && "Memory address out of scope (ALLOCMAPSET)");
      return false;
    }
  if(state)
    {
      alloc_map->bits[mem_offset / alloc_map->word_size] |= On(mem_offset % alloc_map->word_size);
    }
  else
    {
      alloc_map->bits[mem_offset / alloc_map->word_size] &= Off(mem_offset % alloc_map->word_size);
    }
  return true;
}

/*
static void
alloc_map_print_in_use(alloc_map_t *alloc_map)
{
  printf("\nStart addr: %p\n", alloc_map->start_addr);
  printf("Word size: %lu\n", alloc_map->word_size);
  printf("Map size:  %lu\n", alloc_map->map_size);
  for(size_t i = 0; i < alloc_map->map_size; ++i)
    {
      (i%16 == 0)? printf("\n"): 1+1 ;
      (i%256 == 0)? printf("\n"):  1+1 ;


      void * test_ptr = (void *)((size_t)(alloc_map->start_addr) + (i*alloc_map->word_size));

      alloc_map_ptr_used(alloc_map, test_ptr) ?  printf("1") : printf("0");
    }
  printf("\n");
}
*/
