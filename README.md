# gba-flashcartio

![flashcartio](docs/flashcartio.gif)

A Game Boy Advance (GBA) C library to access the SD card of the following flashcarts:
- Everdrive GBA X5
- EZ Flash Omega

The flashcart type is autodetected and \[ex\]FAT partitions are supported via the [ELM-ChaN's FatFs library](http://elm-chan.org/fsw/ff).

**Only reads are supported**.

## Usage

Check out the [example](example/src/main.cpp).