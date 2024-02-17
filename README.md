# gba-flashcartio

![flashcartio](docs/flashcartio.gif)

A Game Boy Advance (GBA) C library to access the SD card of the following flashcarts:
- Everdrive GBA X5
- EZ Flash Omega

The flashcart type is autodetected and \[ex\]FAT partitions are supported via the [ELM-ChaN's FatFs library](http://elm-chan.org/fsw/ff).

**Only reads are supported**.

## Usage

Refer to the [example](example/src/main.cpp) to see how it works. The example is written in C++ for demonstration purposes, but `gba-flashcartio` is a C library, fully compatible with both C and C++.

## Compile-time options

- `FLASHCARTIO_USE_DMA1`: Uncomment this in `lib/sys.h` to use DMA1 instead of DMA3. You'll need this if you also use DMA for audio, as DMA1 has higher priority and will corrupt the SD reads/writes.

## Thanks to

- **asie** for the FatFs library recommendation.
- **Xilefian** for [ezfo-disk_io](https://github.com/felixjones/ezfo-disc_io) development.
- **TotalJustice** for EZfo code improvements in [this gist](https://gist.github.com/ITotalJustice/b6c2f630c6ac5fff1e8b117681e27abd).
