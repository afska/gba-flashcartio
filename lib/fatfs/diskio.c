/*-----------------------------------------------------------------------*/
/* Low level, read-only, Flashcart I/O module for FatFs                  */
/*-----------------------------------------------------------------------*/

#include "../flashcartio.h"
#include "../sys.h"

#include "ff.h" /* Obtains integer types */

#include "diskio.h" /* Declarations of disk functions */

#define ALIGNED __attribute__((aligned(4)))

static u8 EWRAM_BSS aligned_buff[512 * 4] ALIGNED;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE driveId) {
  return driveId == 0 ? 0 : STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE driveId) {
  return active_flashcart != NO_FLASHCART ? 0 : STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
  if ((u32)buff & 0x1) {
    for (UINT i = 0; i < count; i += 4) {
      const u16 blocks = (count - i > 4) ? 4 : (count - i);

      if (!flashcartio_read_sector(sector + i, aligned_buff, blocks))
        return RES_ERROR;

      for (u32 j = 0; j < blocks * 512; j++)
        buff[i * 512 + j] = aligned_buff[j];
    }
    return RES_OK;
  } else {
    return flashcartio_read_sector(sector, buff, count) ? RES_OK : RES_ERROR;
  }
}
