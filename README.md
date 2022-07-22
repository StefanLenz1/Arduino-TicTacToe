# Arduino-TicTacToe
Play TicTacToe against a smart AI \
Because of the limitation of an 8 * 8 Field instead of a cross and circles I use squares and lines. \

## Components
Elegoo UNO R3\
MAX7219\
Membrane Switch Module 4 * 4\

## How to build PlatformIO based project

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-ststm32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd Arduino-TicTacToe

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e maple

# Upload firmware for the specific environment
$ pio run -e maple --target upload

# Clean build files
$ pio run --target clean
```

### Setup
![Setup](https://user-images.githubusercontent.com/87242588/179577344-ec5f96e5-0735-431a-9226-556c61c2bb4c.jpg)

### During a Round
![During_a_Round](https://user-images.githubusercontent.com/87242588/179577353-27993cdc-0563-4b1f-90d4-92c27ecca9d0.jpg)

### Square winning animation
![Square_winning_animation](https://user-images.githubusercontent.com/87242588/179577330-aed2e214-2192-4e09-b15f-ae50f0b88d00.jpg)

### Line winning animation
![Square_winning_animation](https://user-images.githubusercontent.com/87242588/179577340-663eb7f8-7075-4c3a-8734-c0f517d64a1e.jpg)
