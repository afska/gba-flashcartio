# gba-flashcartio

A Game Boy Advance (GBA) C library to access the SD card of the following flashcarts:
- Everdrive GBA X5 / X5Mini
- EZ Flash Omega / OmegaDE

The flashcart type is autodetected and FAT partitions are supported via [ELM-ChaN's FatFs library](http://elm-chan.org/fsw/ff).

- **Only read operations are implemented in FatFs**.
- It reads using either DMA3 or DMA1.
- In EZ Flash mode, ROM is unavailable while using the SD card, so:
  * ~1KB of static EWRAM will be taken by some functions.
  * When reading, interrupts will be briefly disabled (`REG_IME = 0`) to avoid problems.
- I didn't find any licensing terms for the Everdrive code, so if you aim to sell your game, use with caution.

## Usage

Refer to the [example](example/src/main.cpp) to see how it works. It is written in C++ for demonstration purposes, but `gba-flashcartio` is a C library, fully compatible with both C and C++.

## Compile-time options

- `FLASHCARTIO_USE_DMA1`: Uncomment this in `lib/sys.h` to use DMA1 instead of DMA3. You'll need this if you also use DMA for audio, as DMA1/DMA2 have higher priority and will corrupt the SD reads.
- `FF_FS_EXFAT`: exFAT support can be disabled in `lib/fatfs/ffconf.h`, as this can cause patent issues especially for commercial homebrew.

## Thanks to

- **asie** for the FatFs library recommendation.
- **Xilefian** for the [ezfo-disk_io](https://github.com/felixjones/ezfo-disc_io) development.
- **TotalJustice** for EZfo code improvements in [this gist](https://gist.github.com/ITotalJustice/b6c2f630c6ac5fff1e8b117681e27abd).
