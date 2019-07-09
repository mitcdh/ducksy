# Ducksy

A sketch to make your [Teensy 3.6](https://www.pjrc.com/store/teensy36.html) Quack! Reads a plain text file from an SD card connected to the built-in slot on the Teensy, interprets commands, and sends HID commands to the connected host.

## Supported Commands

Supports all of the commands from [Duckyscript](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript) (from the [USB-Rubber-Ducky](https://github.com/hak5darren/USB-Rubber-Ducky) project) interpreter with some additions due to the easy support of mouse movement and file system access. All commands are by default read from a `payload.txt` file that needs to be located on the root of the SD card.

### New Commands

* `MOUSE`: provides for mouse three button click and movement tracking
  * `MOUSE CLICK (MOUSE_LEFT|MOUSE_MIDDLE|MOUSE_RIGHT)`: Press and release a defined mouse button
  * `MOUSE TOGGLE (MOUSE_LEFT|MOUSE_MIDDLE|MOUSE_RIGHT)`: Toggle a defined mouse button, holds the button down until the next toggle command
  * `MOUSE MOVE [X] [Y]`: Provides relative movement from current position based on x and y coordinates
  * `MOUSE MOVETO [X] [Y]`: Locates the mouse at an absolute position based on the defined `RESOLUTION`
* `STRINGLN`: Same as `STRING` but ends with a newline to save on `ENTER` entries
* `RESOLUTION [X] [Y]`: Supports setting the monitor resolution for `MOUSE MOVETO`
* `FILE`: Types out the contents of a file stored on the SD card

## Setup

### Requirements

* [Arduino IDE](https://www.arduino.cc/en/main/software) (developed on 1.8.9)
* [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) (developed on 1.46)
* [SdFat](https://github.com/greiman/SdFat) library (developed on 1.1.0) for Arduino and an FAT16, FAT32, or exFAT formatted SD card

### Deploying with Arduino IDE

1. Install Arduino IDE
2. Install the Teensyduino addon for Arduino
3. `File->Open` the `ducksy.ino` file in the Arduino IDE
4. `Tools->Board` and set to `Teensy 3.6`
5. `Tools->USB Type` and set to `Keyboard + Mouse + Joystick`
6. `Tools->Manage Libraries` and install `SdFat`
7. `Sketch->Include Library` and select `SdFat` to add the SdFat header includes to the top of the sketch
8. Upload to the Teensy!