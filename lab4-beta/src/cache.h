#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdint.h>

#define CACHE_BLOCK_SIZE 64
#define INST_CACHE_SIZE 8192
#define INST_CACHE_WAYS 4
#define INST_CACHE_SETS 64
#define DATA_CACHE_SIZE 65536
#define DATA_CACHE_WAYS 8
#define DATA_CACHE_SETS 256

typedef struct {
    uint32_t tag;
    uint32_t lru;
    uint32_t valid;
    uint32_t dirty;
    uint8_t  data[CACHE_BLOCK_SIZE];
} Cache_Block;

typedef struct {
    Cache_Block blocks[INST_CACHE_WAYS];
} Inst_Cache_Set;

typedef struct {
    Cache_Block blocks[DATA_CACHE_WAYS];
} Data_Cache_Set;

typedef struct {
    Inst_Cache_Set sets[INST_CACHE_SETS];
} Inst_Cache;

typedef struct {
    Data_Cache_Set sets[DATA_CACHE_SETS];
} Data_Cache;

void     inst_cache_init();
uint32_t inst_cache_hit(uint32_t address);
uint32_t inst_cache_read(uint32_t address);

void     data_cache_init();
uint32_t data_cache_hit(uint32_t address);
uint32_t data_cache_read(uint32_t address);
void     data_cache_write(uint32_t address, uint32_t value);

#endif