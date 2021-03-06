#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"

#define Lists 4

typedef struct int_cell int_cell;

struct int_cell
{
  int value;
};

#ifdef GC
#include "../../gc.h"
heap_t *heap; 
#endif

static inline int_cell *populate_lists(list **lists, uint no_lists, uint list_max)
{
  // Choose a list to insert into (simplification from spec)
  list *l = lists[rand() % no_lists];

  // Build an int cell (deviation from spec)
#ifdef GC
  int_cell *c = h_alloc_data(heap, sizeof(int_cell));
#else
  int_cell *c = malloc(sizeof(int_cell));
#endif
  c->value = rand() % list_max;

  // Insert cell in the list
  list_append(l, c);

  return c;
}

/// What happens to performance if we search based on 
/// a == b rather than a->value == b->value? 
/// Hint: The ids array in main holds all ids of the
/// cells if you want to test. 
int int_cell_comparator(void *a, void *b)
{
  return ((int_cell *)a)->value - ((int_cell *)b)->value;
}

static inline bool do_search(list *l, uint list_max)
{
  int_cell target = (int_cell) { .value = rand() % list_max };

  return list_contains(l, &target, int_cell_comparator);
}


int main(int argc, char *argv[])
{
  if (argc < 3)
    {
      printf(
             "Usage: %s M N\n\n"
             "where\n"
             "M is total number of elements in lists\n"
             "N is number of searches performed\n",
             argv[0]
           );
      return 0;
    }

  uint M = atol(argv[1]);
  uint N = atol(argv[2]);

  printf("Searching %d times in %d elements\n", N, M);
  
#ifdef GC
  /// An approximation of 2x memory, adjust if neeced
  const uint HEAP_MAX = 6 * Lists * M * sizeof(void *) + Lists * 6 * sizeof(void *);
  heap = h_init(HEAP_MAX - HEAP_MAX % 2048 + 2048, true, 0.5);
#endif

  list *lists[Lists];

  for (int i = 0; i < Lists; ++i)
    {
      lists[i] = list_new();
    }

  /// Keeping track of all int cells not needed
  int_cell **ids = malloc(M * sizeof(int_cell *));
  int_cell **cursor = ids;
  for (int i = 0; i < M; ++i)
    {
      printf("M = %i\n", i);
      *cursor++ = populate_lists(lists, Lists, M * Lists);
    }

#ifdef GC
  size_t cleaned = h_gc(heap);
  printf("\nCleaned: %lu\n", cleaned);
#endif

  /// What happens to performance if we do all searches in list 0
  /// first, then list 1, etc.? Especially interesting if each
  /// list fits in last-level cache.
  uint hits = 0;
  for (int j = 0; j < N; ++j)
    {
      printf("N = %i\n", j);
      if (do_search(lists[rand() % Lists], M * Lists))
        {
          ++hits;
        }
    }
  printf("Number of elements found: %d\n", hits);
  
#ifdef GC
  h_delete(heap);
#endif

  return 0;
}
