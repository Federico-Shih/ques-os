#ifdef BUDDY

#include "memoryManager.h"
#include "buddyBTree.h"
#include "console.h"

// min 16 bytes -> 2^4
#define MIN_ALLOC_LOG2 4

// max 2GB -> 2^32
#define MAX_ALLOC_LOG2 31

// Usado para establecer la mayor cantidad de ordenes de bloques
#define MAX_BLOCK_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2)

/* Array donde cada entrada representa un orden. Dentro, se guarda una lista para ese orden
   La entrada 0 representa el orden maximo y la ultima el minimo */
static list_t orders[MAX_BLOCK_COUNT];
static uint8_t currentBlocks;
static size_t blockMaxSize;

static list_t *base;

static size_t getMinimumFittingOrder(size_t memorySize);
static void addNodeToOrder(list_t *blockList, list_t *node, uint8_t blockLevel);
static int getFreeBlock(uint8_t minBucketRequired);
static list_t *getBuddyBlock(list_t *node);
static list_t *getBlockAddress(list_t *node);
static int log2(uint32_t number);



void initializeMemoryManager (void *heap_base, unsigned int heap_size)
{
    if(heap_base == NULL) return;
    //inicializo punteros generales
    base = (list_t *)heap_base;

    //tamano maximo del orden mas grande
    blockMaxSize = heap_size;
    //cantidad de bloques disponibles (en total)
    currentBlocks = (int)log2(heap_size) - MIN_ALLOC_LOG2 + 1;

    if(currentBlocks > MAX_BLOCK_COUNT)
        currentBlocks = MAX_BLOCK_COUNT;
    
    for(int i = 0 ; i < currentBlocks; i++)
    {
        // inicializo cada posicion del array con una lista
        // la cual esta libre y representa los bloques de orden i
        initializeList(&orders[i]);
        orders[i].free = 0;
        orders[i].order = i;
    }
    // agrego el primer bloque
    addNodeToOrder(&orders[currentBlocks - 1], base, currentBlocks - 1);
}

void *malloc(uint64_t size)
{
    // necesito espacio necesario mas el tamano del nodo de la lista
    size_t memNeeded = size + sizeof(list_t);
    
    if(size == 0 || memNeeded > blockMaxSize + 1)
        return NULL;

    //primero busco en que orden es mas correcto agregar ese bloque.
    uint8_t lowestFittingOrder = getMinimumFittingOrder(memNeeded);
    //dentro de ese orden, busco un nodo en la lista
    int freeBlock = getFreeBlock(lowestFittingOrder);

    if(freeBlock == -1) return NULL;

    
    list_t *node = popFromList(&orders[freeBlock]);

    // divido el bloque libre(agrego nodos a la lista) en bloques mas pequeños hasta encontrar el bloque ideal
    for( ; lowestFittingOrder < freeBlock ; freeBlock--)
    {
        node->order--;
        addNodeToOrder(&orders[freeBlock - 1], getBuddyBlock(node), freeBlock - 1);
    }

    node->free = 0;

    return (void*)(node + 1);
}

void free(void *block)
{
    if(block == NULL) return;
    
    list_t *freeNode = (list_t *)block - 1;
    freeNode->free = 1;

    list_t *freeBuddyBlock = getBuddyBlock(freeNode);

    // si libero bloques, uno los que quedaron vacios
    while(freeNode->order != currentBlocks - 1 && freeBuddyBlock->order == freeNode->order && freeBuddyBlock->free)
    {
        removeFromList(freeBuddyBlock);
        freeNode = getBlockAddress(freeNode);
        freeNode->order++;
        freeBuddyBlock = getBuddyBlock(freeNode);
    }

    pushToList(&orders[freeNode->order], freeNode);
}

void memoryDump() {
  list_t *list, *listAux;
  uint32_t idx = 0;
  uint32_t freeSpace = 0;

  printf("\nMEMORY DUMP (Buddy Memory Manager)\n");
  printf("\nOrdenes con bloques libres:\n\n");

  for (int i = currentBlocks - 1; i >= 0; i--) {
    list = &orders[i];
    if (!listIsEmpty(list)) {
      printf("Orden %d", i + MIN_ALLOC_LOG2);
      printf("Bloques libres de tamano 2^%d", i + MIN_ALLOC_LOG2);
      for (listAux = list->next, idx = 1; listAux != list;
           idx++, listAux = listAux->next) {
        if (listAux->free) {
          printf("Numero de bloque: %d | ", idx);
          printf("Estado: libre\n");
          freeSpace += idx * (1 << (MIN_ALLOC_LOG2 + i));
        }
      }
    }
  }
  printf("\nEspacio libre: %d\n\n", freeSpace);
}

static size_t getMinimumFittingOrder(size_t memorySize)
{
    size_t memoryBase2 = (size_t)log2(memorySize);

    if(memoryBase2 < MIN_ALLOC_LOG2) return 0;

    memoryBase2 -= MIN_ALLOC_LOG2;

    if( memorySize && !(memorySize & (memorySize - 1)))
        return memoryBase2;
    
    return memoryBase2 + 1;
}

void addNodeToOrder(list_t *blockList, list_t *node, uint8_t blockOrder)
{
    node->order = blockOrder;
    node->free = 1;
    pushToList(blockList, node);
}

int getFreeBlock(uint8_t minBlockRequired)
{
    uint8_t freeBlock;

    for(freeBlock = minBlockRequired; freeBlock < currentBlocks && listIsEmpty(&orders[freeBlock]); freeBlock++);

    if(freeBlock > currentBlocks)
        return -1;
    return freeBlock;
}

list_t *getBuddyBlock(list_t *node)
{
    uint8_t order = node->order;
    uintptr_t currentOffset = (uintptr_t)node - (uintptr_t)base;
    intptr_t newOffest = currentOffset ^ (1 << (MIN_ALLOC_LOG2 + order));

    return (list_t *)((uintptr_t)base + newOffest);
}

list_t *getBlockAddress(list_t *node)
{
    uint8_t order = node->order;
    uintptr_t mask = (1 << (MIN_ALLOC_LOG2 + order));
    mask = ~mask;
    uintptr_t currentOffset = (uintptr_t)node - (uintptr_t)base;
    intptr_t newOffest = currentOffset & mask;

    return (list_t *)(newOffest + (uintptr_t)base);
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
