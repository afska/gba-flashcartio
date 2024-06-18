# gba-flashcartio

A Game Boy Advance (GBA) C library to access the SD card of the following flashcarts:
- EverDrive GBA X5 / Mini
- EZ Flash Omega / OmegaDE

The flashcart type is autodetected and FAT partitions are supported via [ELM-ChaN's FatFs library](http://elm-chan.org/fsw/ff).

- **Only read operations are implemented in FatFs**.
- It reads using either **DMA3** or **DMA1**.
- In **EverDrive** mode:
  * The latter 16MB of the ROM is unavailable while reading, so avoid using that part in your interrupt handlers, or your code might crash.
  * For this same reason, avoid calling soft-resetting while `flashcartio_is_reading`.
- In **EZ Flash** mode:
  * ROM is unavailable while using the SD card, so ~1KB of static EWRAM will be taken by some functions.
  * When reading, by default (see _Compile-time options_ below), interrupts will be briefly disabled (`REG_IME = 0`) to avoid problems.
  * The _EZ Flash Definitive Edition_ works great out of the box, but in the original one:
    * There's an autosave feature that copies the save file to the microSD card. It's triggered every time you write to SRAM, and it takes ~10 seconds to complete. This can cause conflicts if it tries to write the microSD card while you are reading from it. After writing to SRAM, let some time pass before you read the microSD card, or it will crash!
    * When reading, your ROM wait states should be _3,2_ or slower. _3,1_ will certainly make it crash!

## Install

The code for the _EverDrive_ cartridge is publicly available, but it doesn't have any license, so it's not included here.

- By default, the library only works with the _EZ Flash Omega_ cartridges.
- If you want _EverDrive_ support:
  - Download https://krikzz.com/pub/support/everdrive-gba/development/gbaio.zip from krikzz's website.
  - Ensure it's the `05.09.16` version in `main.c`.
  - Put the files `bios.c`, `bios.h`, `disk.c`, `disk.h` in the `lib/everdrivegbax5` directory.
  - In `bios.c`:
    - remove all `(u8*)` and `(u8 *)` casts used in lvalues (left side of assignments).
    - add `| CFG_BIG_ROM` to the first assignment of `bi_init()`.
    - completely remove `bi_reboot(...)` (also its header in the `bios.h` file).
  - In `disk.c`:
    - add the line `#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"` at the top.
  - Set `FLASHCARTIO_ED_ENABLE` in `lib/sys.h` to enable _EverDrive_ support.

## Usage

Refer to the [example](example/src/main.cpp) to see how it works. It is written in C++ for demonstration purposes, but `gba-flashcartio` is a C library, fully compatible with both C and C++.

The example has _EverDrive_ support enabled for demonstration purposes. To disable it, remove the `-DFLASHCARTIO_ED_ENABLE=1` flag in its `Makefile`.

An already compiled .gba ROM is available in the [Releases](https://github.com/afska/gba-flashcartio/releases) section.

## Compile-time options

In `lib/sys.h`:
- `FLASHCARTIO_USE_DMA1`: Set to `1` to use DMA1 instead of DMA3. You'll need this if you also use DMA for audio, as DMA1/DMA2 have higher priority and will corrupt the SD reads.
- `FLASHCARTIO_ED_ENABLE`: (*EverDrive*) Set to `1` to enable _EverDrive_ support.
- `FLASHCARTIO_ED_SAVE_TYPE`: (*EverDrive*) Set your game's save type manually here (one of `BI_SAV_EEP`, `BI_SAV_SRM`, `BI_SAV_FLA64`, `BI_SAV_FLA128`). The default is `BI_SAV_SRM` (SRAM). Unfortunately, this is required for the EverDrive cart, as initializing the registers overwrites ROM configuration that is usually autodetected otherwise.
- `FLASHCARTIO_ED_BIG_ROM`: (*EverDrive*) If your game is bigger than 16MB, this should be enabled (and it is by default).
- `FLASHCARTIO_EZFO_DISABLE_IRQ`: (*EZ Flash*) If you are absolutely sure that your interrupt code doesn't access ROM and you won't be calling `SoftReset` in the middle of a read (see `flashcartio_is_reading`), you can avoid disabling interrupts by setting this option to `0`.

In `lib/fatfs/ffconf.h`:
- `FF_FS_EXFAT`: exFAT support can be disabled, as this can cause patent issues especially for commercial homebrew.

## Thanks to

- **asie** for the FatFs library recommendation.
- **Xilefian** for the [ezfo-disk_io](https://github.com/felixjones/ezfo-disc_io) development.
- **TotalJustice** for EZfo code improvements in [this gist](https://gist.github.com/ITotalJustice/b6c2f630c6ac5fff1e8b117681e27abd).
