#include "flashcartio.h"

#include "everdrivegbax5/bios.h"
#include "everdrivegbax5/disk.h"
#include "ezflashomega/io_ezfo.h"

ActiveFlashcart active_flashcart = NO_FLASHCART;
volatile bool flashcartio_is_reading = false;

bool flashcartio_activate(void) {
  // Everdrive GBA X5
  if (bi_init_sd_only()) {
    bi_init();
    bool success = diskInit() == 0;
    bi_lock_regs();
    if (!success)
      return false;

    active_flashcart = EVERDRIVE_GBA_X5;
    return true;
  }

  // EZ Flash Omega
  if (_EZFO_startUp()) {
    active_flashcart = EZ_FLASH_OMEGA;
    return true;
  }

  return false;
}

bool flashcartio_read_sector(u32 sector, u8* destination, u16 count) {
  switch (active_flashcart) {
    case EVERDRIVE_GBA_X5: {
      flashcartio_is_reading = true;
      bi_unlock_regs();
      bool success = diskRead(sector, destination, count) == 0;
      bi_lock_regs();
      flashcartio_is_reading = false;
      return success;
    }
    case EZ_FLASH_OMEGA: {
      flashcartio_is_reading = true;
      bool success = _EZFO_readSectors(sector, count, destination);
      flashcartio_is_reading = false;
      return success;
    }
    default:
      return false;
  }
}
