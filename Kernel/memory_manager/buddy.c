#ifdef BUDDY

#include "memoryManager.h"
#include "buddyBTree.h"
#include "console.h"

// min 16 bytes -> 2^4
#define MIN_ALLOC_LOG2 4
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_LOG2)

// max 2GB -> 2^32
#define MAX_ALLOC_LOG2 31

#define MAX_BLOCK_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2)

#define BLOCK_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

static list_t blocks[BLOCK_COUNT];
static uint8_t currentBlocks;
static size_t blockMaxSize;

static list_t *base;

static size_t getMinimumFittingBlock(size_t memorySize);
static void addNodeToBlocks(list_t *blockList, list_t *node, uint8_t blockLevel);
static int getFreeBlock(uint8_t minBucketRequired);
static list_t *getBuddyBlock(list_t *node);
static list_t *getBlockAddress(list_t *node);
static int log2(uint32_t number);



void initializeMemoryManager (void *heap_base, unsigned int heap_size)
{
    if(heap_base == NULL)
        return;
    base = (list_t *) heap_base;
    blockMaxSize = heap_size;

    currentBlocks = log2(heap_size) - MIN_ALLOC_LOG2 + 1;

    if(currentBlocks > MAX_BLOCK_COUNT)
        currentBlocks = MAX_BLOCK_COUNT;
    
    for(int i = 0 ; i < currentBlocks; i++)
    {
        initializeList(&blocks[i]);
        blocks[i].free = 0;
        blocks[i].order = i;
    }
    addNodeToBlocks(&blocks[currentBlocks - 1], base, currentBlocks - 1);
}

void *malloc(uint64_t size)
{
    size_t memNeeded = size + sizeof(list_t);
    
    if(memNeeded == 0 || memNeeded > blockMaxSize + 1)
        return NULL;

    uint8_t lowestFittingBlock = getMinimumFittingBlock(memNeeded);
    int freeBlock = getFreeBlock(lowestFittingBlock);

    if(freeBlock == -1)
        return NULL;

    list_t *node = popFromList(&blocks[freeBlock]);

    for( ; lowestFittingBlock < freeBlock ; freeBlock--)
    {
        node->order--;
        addNodeToBlocks(&blocks[freeBlock - 1], getBuddyBlock(node), freeBlock - 1);
    }

    node->free = 0;

    return (void*)(node + 1);
}

void free(void *block)
{
    if(block == NULL)
        return;
    
    list_t *freeNode = (list_t *) block - 1;
    freeNode->free = 1;

    list_t *freeBuddyBlock = getBuddyBlock(freeNode);

    while(freeNode->order != currentBlocks - 1 && freeBuddyBlock->order == freeNode->order && freeBuddyBlock->free)
    {
        removeFromList(freeBuddyBlock);
        freeNode = getBlockAddress(freeNode);
        freeNode->order++;
        freeBuddyBlock = getBuddyBlock(freeNode);
    }

    pushToList(&blocks[freeNode->order], freeNode);
}

void memoryDump() {
  list_t *list, *listAux;
  uint32_t idx = 0;
  uint32_t freeSpace = 0;

  printf("\nMEMORY DUMP (Buddy Memory Manager)\n");
  printf("\nOrdenes con bloques libres:\n\n");

  for (int i = currentBlocks - 1; i >= 0; i--) {
    list = &blocks[i];
    if (!listIsEmpty(list)) {
      printf("    Orden %d\n", i + MIN_ALLOC_LOG2);
      printf("    Bloques libres de tamaño 2^%d\n", i + MIN_ALLOC_LOG2);
      for (listAux = list->next, idx = 1; listAux != list;
           idx++, listAux = listAux->next) {
        if (listAux->free) {
          printf("        Numero de bloque: %d\n", idx);
          printf("        Estado: libre\n\n");
          freeSpace += idx * (1 << (MIN_ALLOC_LOG2 + i));
        }
      }
    }
  }
  printf("\nEspacio libre: %d\n\n", freeSpace);
}

static size_t getMinimumFittingBlock(size_t memorySize)
{
    size_t memoryBase2 = log2(memorySize);

    if(memoryBase2 < MIN_ALLOC_LOG2)
        return 0;

    memoryBase2 -= MIN_ALLOC_LOG2;

    if( memorySize && (memorySize & (memorySize -1)))
        return memoryBase2;
    
    return memoryBase2 + 1;
}

void addNodeToBlocks(list_t *blockList, list_t *node, uint8_t blockOrder)
{
    node->order = blockOrder;
    node->free = 1;
    pushToList(blockList, node);
}
int getFreeBlock(uint8_t minBlockRequired)
{
    uint8_t freeBlock;

    for(freeBlock = minBlockRequired; freeBlock < currentBlocks && listIsEmpty(&blocks[freeBlock]); freeBlock++);

    if(freeBlock > currentBlocks)
        return -1;
    return freeBlock;
}
list_t *getBuddyBlock(list_t *node)
{
    uint8_t block = node->order;
    uintptr_t currentOffset = (uintptr_t) node - (uintptr_t) base;
    intptr_t newOffest = currentOffset ^ (1 << (MIN_ALLOC_LOG2 + block));

    return (list_t *)((uintptr_t) base + newOffest);
}
list_t *getBlockAddress(list_t *node)
{
    uint8_t block = node->order;
    uintptr_t mask = (1 << (MIN_ALLOC_LOG2 + block));
    mask = ~mask;
    uintptr_t currentOffset = (uintptr_t) node - (uintptr_t) base;
    intptr_t newOffest = currentOffset & mask;

    return (list_t *) (newOffest + (uintptr_t)base);
}

int log2(uint32_t number)
{
  if (number == 0) {
    return -1;
  }

  int result = -1;
  while (number) {
    result++;
    number >>= 1;
  }
  return result;
}


#endif
