#include "sys.h"

#include "flashcartio.h"

#if FLASHCARTIO_ED_ENABLE != 0
#include "everdrivegbax5/disk.h"
#include "everdrivegbax5/everdrive.h"
#endif

#if FLASHCARTIO_EZFO_ENABLE != 0
#include "ezflashomega/io_ezfo.h"
#endif

ActiveFlashcart active_flashcart = NO_FLASHCART;
volatile bool flashcartio_is_reading = false;

bool flashcartio_activate(void) {
#if FLASHCARTIO_ED_ENABLE != 0

#if FLASHCARTIO_ED_DISABLE_IRQ != 0
  u16 ime = REG_IME;
  REG_IME = 0;
#endif

  // Everdrive GBA X5
  if (ed_init_sd_only()) {
    ed_init();
    ed_set_save_type(FLASHCARTIO_ED_SAVE_TYPE);
    bool success = diskInit() == 0;
    ed_lock_regs();
    if (!success) {
#if FLASHCARTIO_ED_DISABLE_IRQ != 0
      REG_IME = ime;
#endif

      return false;
    }

    active_flashcart = EVERDRIVE_GBA_X5;

#if FLASHCARTIO_ED_DISABLE_IRQ != 0
    REG_IME = ime;
#endif

    return true;
  }
#endif

#if FLASHCARTIO_EZFO_ENABLE != 0
  // EZ Flash Omega
  if (_EZFO_startUp()) {
    active_flashcart = EZ_FLASH_OMEGA;
    return true;
  }
#endif

  return false;
}

bool flashcartio_read_sector(u32 sector, u8* destination, u16 count) {
  switch (active_flashcart) {
#if FLASHCARTIO_ED_ENABLE != 0
    case EVERDRIVE_GBA_X5: {
#if FLASHCARTIO_ED_DISABLE_IRQ != 0
      u16 ime = REG_IME;
      REG_IME = 0;
#endif

      flashcartio_is_reading = true;
      ed_unlock_regs();
      bool success = diskRead(sector, destination, count) == 0;
      ed_lock_regs();
      flashcartio_is_reading = false;

#if FLASHCARTIO_ED_DISABLE_IRQ != 0
      REG_IME = ime;
#endif

      return success;
    }
#endif
#if FLASHCARTIO_EZFO_ENABLE != 0
    case EZ_FLASH_OMEGA: {
      flashcartio_is_reading = true;
      bool success = _EZFO_readSectors(sector, count, destination);
      flashcartio_is_reading = false;
      return success;
    }
#endif
    default:
      return false;
  }
}
