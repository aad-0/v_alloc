#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "valloc.h"

#define NUM_ALLOCATIONS 200

int main (void)
{
  void* pointers[NUM_ALLOCATIONS];
  clock_t start, end;
  double cpu_time_used;

  v_alloc_init();

  // Allocation test
  start = clock();
  for (int i = 0; i < NUM_ALLOCATIONS; i++) {
    pointers[i] = v_alloc(sizeof(uint8_t) * (rand() % 10 + 1));
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Allocation time for %d blocks: %f seconds\n", NUM_ALLOCATIONS, cpu_time_used);

  // Deallocation test
  start = clock();
  for (int i = 0; i < NUM_ALLOCATIONS; i++) {
    v_alloc_free(pointers[i]);
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Deallocation time for %d blocks: %f seconds\n", NUM_ALLOCATIONS, cpu_time_used);

  return 0;
}
