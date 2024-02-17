#ifndef FLASHCARTIO_H
#define FLASHCARTIO_H

#include <stdbool.h>
#include <stdint.h>

typedef enum { NO_FLASHCART, EVERDRIVE_GBA_X5 } ActiveFlashcart;

extern ActiveFlashcart active_flashcart;

bool flashcartio_activate(void);
bool flashcartio_read_sector(uint32_t sector,
                             uint8_t* destination,
                             uint16_t count);

#endif  // FLASHCARTIO_H
