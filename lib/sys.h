#ifndef SYS_H
#define SYS_H

#include <stdbool.h>

// Use DMA1 instead of DMA3 (uncomment to enable)
// #define FLASHCARTIO_USE_DMA1

#ifdef FLASHCARTIO_USE_DMA1
#define DMA_SRC *((volatile u32*)0x40000BC)
#define DMA_DST *((volatile u32*)0x40000C0)
#define DMA_LEN *((volatile u16*)0x40000C4)
#define DMA_CTR *((volatile u16*)0x40000C6)
#endif
#ifndef FLASHCARTIO_USE_DMA1
#define DMA_SRC *((vu32*)0x40000D4)
#define DMA_DST *((vu32*)0x40000D8)
#define DMA_LEN *((vu16*)0x40000DC)
#define DMA_CTR *((vu16*)0x40000DE)
#endif

#define u8 unsigned char
#define vu8 volatile unsigned char
#define u16 unsigned short
#define vu16 volatile unsigned short
#define u32 unsigned int
#define vu32 volatile unsigned int

#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define EWRAM_BSS __attribute__((section(".sbss")))

#define DMA_ENABLE (1 << 31)
#define DMA16 (0 << 26)
#define DMA32 (1 << 26)
#define DMA_Copy(source, dest, mode) \
  {                                  \
    DMA_SRC = (u32)(source);         \
    DMA_DST = (u32)(dest);           \
    DMA_CTR = DMA_ENABLE | (mode);   \
  }
inline void dmaCopy(const void* source, void* dest, u32 size) {
  DMA_Copy(source, dest, DMA16 | size >> 1);
}

#endif /* SYS_H */
