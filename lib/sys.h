#ifndef SYS_H
#define SYS_H

#include <stdbool.h>

#define u8 unsigned char
#define vu8 volatile unsigned char

#define u16 unsigned short
#define vu16 volatile unsigned short
#define u32 unsigned int
#define vu32 volatile unsigned int

// [!] DMA3 (disabled)
// #define DMA_SRC *((vu32*)0x40000D4)
// #define DMA_DST *((vu32*)0x40000D8)
// #define DMA_LEN *((vu16*)0x40000DC)
// #define DMA_CTR *((vu16*)0x40000DE)
// (replaced with DMA1)
#define DMA_SRC *((volatile u32*)0x40000BC)
#define DMA_DST *((volatile u32*)0x40000C0)
#define DMA_LEN *((volatile u16*)0x40000C4)
#define DMA_CTR *((volatile u16*)0x40000C6)

#endif /* SYS_H */
