#include <stdint.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char* str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);
void * memset(void * destination, int32_t c, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);