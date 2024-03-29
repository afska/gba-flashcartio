﻿# gba-flashcartio

A Game Boy Advance (GBA) C library to access the SD card of the following flashcarts:
- EverDrive GBA X5 / Mini
- EZ Flash Omega / OmegaDE

The flashcart type is autodetected and FAT partitions are supported via [ELM-ChaN's FatFs library](http://elm-chan.org/fsw/ff).

- **Only read operations are implemented in FatFs**.
- It reads using either DMA3 or DMA1.
- In EverDrive mode, the latter 16MB of the ROM is unavailable while reading, so avoid using that part in your interrupt handlers, or your code might crash.
  * For this same reason, avoid calling `SoftReset` while `flashcartio_is_reading`.
- In EZ Flash mode, ROM is unavailable while using the SD card, so:
  * ~1KB of static EWRAM will be taken by some functions.
  * When reading, by default (see _Compile-time options_ below), interrupts will be briefly disabled (`REG_IME = 0`) to avoid problems.
- I didn't find any licensing terms for the EverDrive code, so if you aim to sell your game, use with caution.

## Usage

Refer to the [example](example/src/main.cpp) to see how it works. It is written in C++ for demonstration purposes, but `gba-flashcartio` is a C library, fully compatible with both C and C++.

## Compile-time options

In `lib/sys.h`:
- `FLASHCARTIO_USE_DMA1`: Set to `1` to use DMA1 instead of DMA3. You'll need this if you also use DMA for audio, as DMA1/DMA2 have higher priority and will corrupt the SD reads.
- `FLASHCARTIO_ED_SAVE_TYPE`: (*EverDrive*) Set your game's save type manually here (one of `BI_SAV_EEP`, `BI_SAV_SRM`, `BI_SAV_FLA64`, `BI_SAV_FLA128`). The default is `BI_SAV_SRM` (SRAM). Unfortunately, this is required for the EverDrive cart, as initializing the registers overwrites ROM configuration that is usually autodetected otherwise.
- `FLASHCARTIO_ED_BIG_ROM`: (*EverDrive*) If your game is bigger than 16MB, this should be enabled (and it is by default).
- `FLASHCARTIO_EZFO_DISABLE_IRQ`: (*EZ Flash*) If you are absolutely sure that your interrupt code doesn't access ROM and you won't be calling `SoftReset` in the middle of a read (see `flashcartio_is_reading`), you can avoid disabling interrupts by setting this option to `0`.

In `lib/fatfs/ffconf.h`:
- `FF_FS_EXFAT`: exFAT support can be disabled, as this can cause patent issues especially for commercial homebrew.

## Thanks to

- **asie** for the FatFs library recommendation.
- **Xilefian** for the [ezfo-disk_io](https://github.com/felixjones/ezfo-disc_io) development.
- **TotalJustice** for EZfo code improvements in [this gist](https://gist.github.com/ITotalJustice/b6c2f630c6ac5fff1e8b117681e27abd).
