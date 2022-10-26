#ifdef STANDARD

#include <memoryManager.h>
#include <stdlib.h>
#include <stdbool.h>
#include <console.h>

// La estructura fue inspirada de la implementación de malloc del K&R 
typedef long Align;

typedef union header
{
  struct
  {
    union header *next;
    unsigned int size;
  } data;
  Align x;
} Header;

static Header *base;
static Header *free_node = NULL;

static unsigned int header_blocks;

void *malloc(uint64_t mem)
{
  if (mem == 0)
    return NULL;
  Header *current, *prev;
  size_t blocks = (mem + sizeof(Header) - 1) / sizeof(Header) + 1;

  void *res;
  bool end = false;

  prev = free_node;
  for (current = free_node->data.next; !end; current = prev->data.next)
  {
    if (current->data.size >= blocks)
    {
      if (current->data.size == blocks)
      {
        prev->data.next = current->data.next;
      }
      else
      {
        current->data.size -= blocks;
        current += current->data.size;
        current->data.size = blocks;
      }
      free_node = prev;
      res = current + 1;
      end = true;
    }
    if (current == free_node && !end)
    {
      return NULL;
    }
    prev = current;
  }

  return res;
}

void free(void *memory)
{
  if (memory == NULL || (((long)(memory) - sizeof(Header) - (long)base) % sizeof(Header)) != 0)
    return;
  Header *free_block, *current_block;
  free_block = (Header *)memory - 1;

  if (free_block < base || free_block >= base + header_blocks * sizeof(Header))
  {
    return;
  }

  bool valid_external = false;
  for (current_block = free_node; 
      !(free_block > current_block && free_block < current_block->data.next); 
      current_block = current_block->data.next)
  {
    if (current_block == free_block || current_block->data.next == free_block)
      return;
    if (current_block >= current_block->data.next && 
      (free_block > current_block || free_block < current_block->data.next))
    {
      valid_external = true;
      break;
    }
  }

  if (!valid_external && 
    (current_block + current_block->data.size > free_block 
      || free_block + free_block->data.size > current_block->data.next)) {
    return;
  }

  // Encontramos un bloque y unir
  if (free_block + free_block->data.size == current_block->data.next) {
    free_block->data.size += current_block->data.next->data.size;
    free_block->data.next = current_block->data.next->data.next;
  } else {
    free_block->data.next = current_block->data.next;
  }

  if (current_block + current_block->data.size == free_block) {
    current_block->data.size += free_block->data.size;
    current_block->data.next = free_block->data.next;
  } else {
    current_block->data.next = free_block;
  }

  free_node = current_block;
}

void initializeMemoryManager(void *heap_base, unsigned int heap_size)
{
  if (heap_base == NULL)
    return;
  header_blocks = (heap_size + sizeof(Header) - 1) / sizeof(Header) + 1;
  free_node = base = (Header *)heap_base;
  free_node->data.size = header_blocks;
  free_node->data.next = free_node;
}

/*
  Memoria
  total:
  ocupada:
  libre:
*/

void memoryDump()
{
  print("\nMemory manager dump (Free List)", 33);
  print("\nMemoria total:", 16);
  uint64_t memoria_total = header_blocks * sizeof(Header);
  printBase(memoria_total, 10);
  print("\n\n------------------------------------", 39);

  if (free_node == NULL) {
    print("\nNo hay bloques libres", 23);
    return;
  }

  print("\nBloques libres\n\n", 16);

  Header* current, *free_base;

  current = free_base = free_node;
  int i = 0;
  unsigned int cant = 0;
  do {
    cant += current->data.size;
    print("   Bloque nro: ", 16);
    printBase(i, 10);
    printChar('\n');
    print("   Base: ", 10);
    printBase((uint64_t)current, 16);
    printChar('\n');
    print("   Cantidad de bloques: ", 23);
    printBase(current->data.size, 10);
    print("\n\n------------------------------------\n", 40);
    current = current->data.next;
    i += 1;
  } while (current != free_base);

  uint64_t memoria_libre_total = cant * sizeof(Header);
  print("\n\n------------------------------------\n\n", 41);
  print("\nMemoria libre: ", 17);
  printBase(memoria_libre_total, 10);
  print("\nMemoria ocupada ", 18);
  printBase((memoria_total - memoria_libre_total), 10);
  return;
}


#endif