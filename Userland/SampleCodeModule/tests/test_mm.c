// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define MAX_BLOCKS 128

#include <stdint.h>
#include "test_util.h"
#include "ustdlib.h"

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;

void test_mm(unsigned int argc, char *argv[]){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc < 2) return;

  if ((max_memory = satoi(argv[1])) <= 0) return;

  while (1){
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while(rq < MAX_BLOCKS && total < max_memory){
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = sys_malloc(mm_rqs[rq].size);

      if(mm_rqs[rq].address){
        total += mm_rqs[rq].size;
        rq++;
      }
    }
  
    bussy_wait(MINOR_WAIT);

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)){
          _fprintf("test_mm ERROR\n");
          return;
        }

    bussy_wait(MINOR_WAIT);

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_free(mm_rqs[i].address);
    
    bussy_wait(MINOR_WAIT);
  } 
}