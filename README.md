# gba-flashcartio

A Game Boy Advance (GBA) C library to access the SD card of the following flashcarts:
- EverDrive GBA X5 / Mini
- EZ Flash Omega / OmegaDE

The flashcart type is autodetected and FAT partitions are supported via [ELM-ChaN's FatFs library](http://elm-chan.org/fsw/ff).

- **Only read operations are implemented in FatFs**.
- It reads using either **DMA3** or **DMA1**.
- While reading, flashcarts make part of the ROM inaccessible during the operation (*EverDrive* disables the last 16 MB, and *EZ Flash* disables all ROM space). To prevent issues, by default, interrupts are briefly disabled (`REG_IME = 0`).
- **EverDrive** notes:
  * Since the last 16 MB of ROM are unavailable while using the SD card, make sure your linker script places these functions in the first 16 MB of ROM or in RAM.
- **EZ Flash** notes:
  * Since ROM is unavailable while using the SD card, ~1 KB of static EWRAM will be taken by some functions.
  * The _EZ Flash Definitive Edition_ works great out of the box, but in the original one:
    * There's an autosave feature that copies the save file to the microSD card. It's triggered every time you write to SRAM, and it takes ~10 seconds to complete. This can cause conflicts if it tries to write the microSD card while you are reading from it. After writing to SRAM, let some time pass before you read the microSD card, or it will crash!
    * Your ROM wait states should be `3,2` or slower. `3,1` will certainly make it crash!

## Usage

Refer to the [example](example/src/main.cpp) to see how it works. It is written in C++ for demonstration purposes, but `gba-flashcartio` is a C library, fully compatible with both C and C++.

An already compiled .gba ROM is available in the [Releases](https://github.com/afska/gba-flashcartio/releases) section.

You can compile the example using Docker:

```bash
cd example

docker run -it \
  --user "$(id -u):$(id -g)" \
  -v "$(pwd)/..":/opt/gba \
  -v "$(pwd)":/opt/gba/example \
  devkitpro/devkitarm:20241104 \
  bash -c 'cd /opt/gba/example && make rebuild'
```

## Compile-time options

In `lib/sys.h`:
- `FLASHCARTIO_DISABLE_DMA` (default=`0`): Set to `1` to use regular copies instead of DMA. This is slower but can be helpful in some scenarios.
- `FLASHCARTIO_USE_DMA1` (default=`0`): Set to `1` to use DMA1 instead of DMA3. You'll need this if you also use DMA for audio, as DMA1/DMA2 have higher priority and will corrupt the SD reads. With this option, you can use DMA2 for audio and DMA1 for the SD card.
- `FLASHCARTIO_ED_ENABLE` (default=`1`): (*EverDrive*) Set to `0` to disable _EverDrive_ support.
- `FLASHCARTIO_ED_SAVE_TYPE` (default=`ED_SAVE_TYPE_SRM`): (*EverDrive*) Set your game's save type manually here (one of `ED_SAVE_TYPE_EEP`, `ED_SAVE_TYPE_SRM`, `ED_SAVE_TYPE_FLA64`, `ED_SAVE_TYPE_FLA128`). Unfortunately, this is required, since initializing the registers overwrites ROM configuration that is usually autodetected otherwise.
- `FLASHCARTIO_ED_DISABLE_IRQ` (default=`1`): (*EverDrive*) If you are absolutely sure that your interrupt code doesn't access the last 16 MB of ROM and you won't be calling `SoftReset` in the middle of a read (when `flashcartio_is_reading` is `true`), you can avoid disabling interrupts by setting this option to `0`.
- `FLASHCARTIO_EZFO_ENABLE` (default=`1`): (*EZ Flash*) Set to `0` to disable _EZ Flash_ support.
- `FLASHCARTIO_EZFO_DISABLE_IRQ` (default=`1`): (*EZ Flash*) If you are absolutely sure that your interrupt code doesn't access ROM at all and you won't be calling `SoftReset` in the middle of a read (when `flashcartio_is_reading` is `true`), you can avoid disabling interrupts by setting this option to `0`.

In `lib/fatfs/ffconf.h`:
- `FF_FS_EXFAT`: exFAT support can be disabled, as this can cause patent issues especially for commercial homebrew.

## Thanks to

- **asie** for the FatFs library recommendation.
- **Xilefian** for the [ezfo-disk_io](https://github.com/felixjones/ezfo-disc_io) development.
- **TotalJustice** for EZfo code improvements in [this gist](https://gist.github.com/ITotalJustice/b6c2f630c6ac5fff1e8b117681e27abd).
- **krikzz** for open sourcing [gba-ed-pub](https://github.com/krikzz/gba-ed-pub).
