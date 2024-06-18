#include "flashcartio.h"

#if FLASHCARTIO_ED_ENABLE != 0
#include "everdrivegbax5/bios.h"
#include "everdrivegbax5/disk.h"
#endif

#include "ezflashomega/io_ezfo.h"

ActiveFlashcart active_flashcart = NO_FLASHCART;
volatile bool flashcartio_is_reading = false;

#if FLASHCARTIO_ED_ENABLE != 0
bool detect_everdrive() {
  *((vu16*)(0x9FC0000 + 0xB4)) = 0;
  u16 config = *((vu16*)(0x9FC0000 + 0x14));
  if (config == 0 || config == 0xffff)
    return false;
  *((vu16*)(0x9FC0000 + 0x14)) = 0;
  if (*((vu16*)(0x9FC0000 + 0x14)) != config)
    return false;
  *((vu16*)(0x9FC0000 + 0xB4)) = 0xA5;
  *((vu16*)(0x9FC0000 + 0x14)) = 0;
  if (*((vu16*)(0x9FC0000 + 0x14)) == config)
    return false;

  return true;
}

void setup_everdrive() {
  bi_init();
  bi_set_save_type(FLASHCARTIO_ED_SAVE_TYPE);
}
#endif

bool flashcartio_activate(void) {
#if FLASHCARTIO_ED_ENABLE != 0
  // Everdrive GBA X5
  if (detect_everdrive()) {
    setup_everdrive();
    bool success = diskInit() == 0;
    bi_lock_regs();
    if (!success)
      return false;

    active_flashcart = EVERDRIVE_GBA_X5;
    return true;
  }
#endif

  // EZ Flash Omega
  if (_EZFO_startUp()) {
    active_flashcart = EZ_FLASH_OMEGA;
    return true;
  }

  return false;
}

bool flashcartio_read_sector(u32 sector, u8* destination, u16 count) {
  switch (active_flashcart) {
#if FLASHCARTIO_ED_ENABLE != 0
    case EVERDRIVE_GBA_X5: {
      flashcartio_is_reading = true;
      bi_unlock_regs();
      bool success = diskRead(sector, destination, count) == 0;
      bi_lock_regs();
      flashcartio_is_reading = false;
      return success;
    }
#endif
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
