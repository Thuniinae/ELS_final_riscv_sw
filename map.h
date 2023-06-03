#include "stdlib.h"
#include "stdint.h"

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

union mean_t {
  unsigned char uc[32];
};

union distance_t {
  unsigned char uc[24];
};

// modules address
static char* const GD_W_RGB_ADDR = reinterpret_cast<char* const>      (0x73000000);
static char* const GD_W_MEAN_ADDR = reinterpret_cast<char* const>     (0x73000004);
static char* const GD_R_DISTANCE_ADDR  = reinterpret_cast<char* const>(0x73000024);
static char* const AM_W_DISTANCE_ADDR  = reinterpret_cast<char* const>(0x74000000);
static char* const AM_R_INDEX_ADDR  = reinterpret_cast<char* const>   (0x74000018);
static char* const NM_W_INDEX_ADDR  = reinterpret_cast<char* const>   (0x75000000);
static char* const NM_W_RGB_ADDR  = reinterpret_cast<char* const>     (0x75000001);
static char* const NM_R_MEAN_ADDR  = reinterpret_cast<char* const>    (0x75000005);
static char* const CP_W_INDEX_ADDR  = reinterpret_cast<char* const>   (0x76000000);
static char* const CP_W_MEAN_ADDR  = reinterpret_cast<char* const>    (0x76000001);
static char* const CP_R_RGB_ADDR  = reinterpret_cast<char* const>     (0x76000021);



// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;