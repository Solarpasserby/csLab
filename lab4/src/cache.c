#include "cache.h"
#include "shell.h"
#include "pipe.h"
#include <stdio.h>
#include <stdint.h>

Inst_Cache inst_cache;
Data_Cache data_cache;

void inst_cache_init()
{
    memset(&inst_cache, 0, sizeof(Inst_Cache));
}

uint32_t inst_cache_hit(uint32_t address)
{
    uint32_t set  = (address >> 5) & 0x3F;
    uint32_t tag  = (address >> 11) & 0x1FF;

    for (int i = 0; i < INST_CACHE_WAYS; i++) {
        if (inst_cache.sets[set].blocks[i].valid && inst_cache.sets[set].blocks[i].tag == tag) {
            return i;
        }
    }

    return UINT32_MAX;
}

uint32_t inst_cache_read(uint32_t address)
{
    uint32_t offset  = address & 0x1F;
    uint32_t set     = (address >> 5) & 0x3F;
    uint32_t tag     = (address >> 11) & 0x1FF;

    uint32_t way = inst_cache_hit(address);

    if (way == UINT32_MAX) {
        way = 0;
        for (int i = 0; i < INST_CACHE_WAYS; i++) {
            if (inst_cache.sets[set].blocks[i].valid && inst_cache.sets[set].blocks[i].lru > inst_cache.sets[set].blocks[way].lru) {
                way = i;
            }
        }

        address &= ~0x1F;
        for (int i = 0; i < CACHE_BLOCK_SIZE; i++) {
            inst_cache.sets[set].blocks[way].data[i] = mem_read_32(address + i);
        }

        inst_cache.sets[set].blocks[way].tag = tag;
        inst_cache.sets[set].blocks[way].lru = 0;
        inst_cache.sets[set].blocks[way].dirty = 0;
        inst_cache.sets[set].blocks[way].valid = 1;
    }

    for (int i = 0; i < INST_CACHE_WAYS; i++) {
        if (inst_cache.sets[set].blocks[i].valid && inst_cache.sets[set].blocks[i].lru < inst_cache.sets[set].blocks[way].lru) {
            inst_cache.sets[set].blocks[i].lru++;
        }
    }

    inst_cache.sets[set].blocks[way].lru = 0;

    return inst_cache.sets[set].blocks[way].data[offset];
}

void data_cache_init()
{
    memset(&data_cache, 0, sizeof(Data_Cache));
}

uint32_t data_cache_hit(uint32_t address)
{
    uint32_t set = (address >> 5) & 0xFF;
    uint32_t tag = (address >> 13) & 0x7F;

    for (int i = 0; i < DATA_CACHE_WAYS; i++) {
        if (data_cache.sets[set].blocks[i].valid && data_cache.sets[set].blocks[i].tag == tag) {
            return i;
        }
    }

    return UINT32_MAX;
}

uint32_t data_cache_read(uint32_t address)
{
    uint32_t offset  = address & 0x1F;
    uint32_t set     = (address >> 5) & 0xFF;
    uint32_t tag     = (address >> 13) & 0x7F;

    uint32_t way = data_cache_hit(address);

    if (way == UINT32_MAX) {
        way = 0;
        for (int i = 0; i < DATA_CACHE_WAYS; i++) {
            if (data_cache.sets[set].blocks[i].valid && data_cache.sets[set].blocks[i].lru > data_cache.sets[set].blocks[way].lru) {
                way = i;
            }
        }

        if (data_cache.sets[set].blocks[way].dirty) {
            uint32_t old_tag = data_cache.sets[set].blocks[way].tag;
            uint32_t old_address = (old_tag << 13) | (set << 5) | MEM_DATA_START;
            for (int i = 0; i < CACHE_BLOCK_SIZE; i++) {
                mem_write_32(old_address + i, data_cache.sets[set].blocks[way].data[i]);
            }
        }

        address &= ~0x1F;
        for (int i = 0; i < CACHE_BLOCK_SIZE; i++) {
            data_cache.sets[set].blocks[way].data[i] = mem_read_32(address + i);
        }

        data_cache.sets[set].blocks[way].tag = tag;
        data_cache.sets[set].blocks[way].lru = 0;
        data_cache.sets[set].blocks[way].dirty = 0;
        data_cache.sets[set].blocks[way].valid = 1;
    }

    for (int i = 0; i < DATA_CACHE_WAYS; i++) {
        if (data_cache.sets[set].blocks[i].valid && data_cache.sets[set].blocks[i].lru < data_cache.sets[set].blocks[way].lru) {
            data_cache.sets[set].blocks[i].lru++;
        }
    }

    data_cache.sets[set].blocks[way].lru = 0;

    return data_cache.sets[set].blocks[way].data[offset];
}

void data_cache_write(uint32_t address, uint32_t value)
{
    uint32_t offset  = address & 0x1F;
    uint32_t set     = (address >> 5) & 0xFF;
    uint32_t tag     = (address >> 13) & 0x7F;

    uint32_t way = data_cache_hit(address);

    if (way == UINT32_MAX) {
        way = 0;
        for (int i = 0; i < DATA_CACHE_WAYS; i++) {
            if (data_cache.sets[set].blocks[i].valid && data_cache.sets[set].blocks[i].lru > data_cache.sets[set].blocks[way].lru) {
                way = i;
            }
        }

        if (data_cache.sets[set].blocks[way].dirty) {
            uint32_t old_tag = data_cache.sets[set].blocks[way].tag;
            uint32_t old_address = (old_tag << 13) | (set << 5) | MEM_DATA_START;
            for (int i = 0; i < CACHE_BLOCK_SIZE; i++) {
                mem_write_32(old_address + i, data_cache.sets[set].blocks[way].data[i]);
            }
        }

        address &= ~0x1F;
        for (int i = 0; i < CACHE_BLOCK_SIZE; i++) {
            data_cache.sets[set].blocks[way].data[i] = mem_read_32(address + i);
        }

        data_cache.sets[set].blocks[way].tag = tag;
        data_cache.sets[set].blocks[way].lru = 0;
        data_cache.sets[set].blocks[way].dirty = 0;
        data_cache.sets[set].blocks[way].valid = 1;
    }

    for (int i = 0; i < DATA_CACHE_WAYS; i++) {
        if (data_cache.sets[set].blocks[i].valid && data_cache.sets[set].blocks[i].lru < data_cache.sets[set].blocks[way].lru) {
            data_cache.sets[set].blocks[i].lru++;
        }
    }

    data_cache.sets[set].blocks[way].lru = 0;

    data_cache.sets[set].blocks[way].data[offset] = value;
}