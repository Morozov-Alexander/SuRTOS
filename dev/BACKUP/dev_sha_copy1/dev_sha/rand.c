
#include "rand.h"
#include "sha256.h"

#define BASE_SEED_SIZE      (32)

static volatile uint8_t _seed[BASE_SEED_SIZE] = 
{
    0x12, 0x22, 0xAF, 0xAC, 0xBA, 0xFC, 0xAC, 0x44,
    0x12, 0x22, 0xAF, 0xAC, 0xBA, 0xFC, 0xCD, 0x00,
    0x12, 0x22, 0xAF, 0x36, 0xBA, 0xFC, 0x11, 0xFF,
    0x12, 0x22, 0xAF, 0xAC, 0xBA, 0xFC, 0x00, 0x5c
};

void srand(uint32_t seed)
{
    *((uint32_t*)_seed) ^= (((seed * 214013L + 2531011L) >> 16) & 0x7fff);
    *((uint32_t*)_seed + 1) ^= (seed * 211113L + 2531011L);
    *((uint32_t*)_seed + 2) ^= (seed * 210213L + 9121019L);
    *((uint32_t*)_seed + 3) ^= (seed * 331217L + 3291227L) + *((uint32_t*)_seed);
    *((uint32_t*)_seed + 4) ^= (seed * 214017L + 2531011L);
    *((uint32_t*)_seed + 5) ^=  ~*((uint32_t*)_seed + 2) ^ ~ *((uint32_t*)_seed + 4);
    *((uint32_t*)_seed + 6) ^=  *((uint32_t*)_seed + 6) + *((uint32_t*)_seed + 1);
    *((uint32_t*)_seed + 7) += 0xFA13;
}
int32_t rand()
{
    int32_t res = 0;
#ifndef RAND_FAST
    uint8_t hash[64];

    sha256(_seed, BASE_SEED_SIZE, hash);
   
    res = (TO_U32(hash[3]) << 24) | (TO_U32(hash[23]) << 16) 
        | (TO_U32(hash[7]) << 8) | (TO_U32(hash[16]));
    for (uint32_t i = 0; i < BASE_SEED_SIZE; ++i)
    {
        _seed[i] ^= hash[i] + 0xff7f;
        LEFT_ROTATE(_seed[i], _seed[i] & 0x10);
    }
#else
    *((uint32_t*)_seed) = ((*((uint32_t*)_seed) * 214013L + 2531011L) >> 16) & 0x7fff;
    for (uint32_t i = 1; i < 8; i++)
    {
        *((uint32_t*)_seed + i) ^= (*((uint32_t*)_seed + i - 1) * 214013L + 2531011L);
        LEFT_ROTATE(_seed[i], 16);

    }
    res = (TO_U32(_seed[3]) << 24) | (TO_U32(_seed[23]) << 16) 
        | (TO_U32(_seed[7]) << 8) | (TO_U32(_seed[16]));
#endif
    return res;
}
