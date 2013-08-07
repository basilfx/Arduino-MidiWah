# MidiWah v1.0
Arduino based bridge between the wah-wah (e.g. Dunlop GCB-95) and its potentiometer. Digitizes the potentiometer and feeds it via a digital potentiometer back to the circuit.

As a bonus, the digitized potentiometer signal can be  outputted via MIDI OUT.

## Contents
* Eagle schematic and PCB
* Arduino firmware

## Eagle

### Bill of materials
* Atmega168/328
* MCP4251
* 6N138

To be done.

### Board render
To be done.

## Arduino
The source can be found under `firmware`. It's build with Arduino 1.0.5, but newer versions should work too.

See the `firmware/UsedLibraries.md` for the libraries to install.

### Firmware
To be done.

### Configuration
The DIP switch on the PCB select the channel to which the MidiWah will respond

The firmware can be programmed via a few MIDI Control Change messages. Changes are only persisted when a save command is issued.

* `CC 80 [0|1]` &mdash; disable or enable calibration of the potentiometer. Replies with the same value.
* `CC 81 [0|1]` &mdash; disable or enable invertion of the potentiometer value. Replies with the same value.
* `CC 82 [0-2]` &mdash; select potentiometer program. Replies with the selected program, or the last one if program is invalid.
  * Program 0 is linear
  * Program 1 is logarithmic
  * Program 3 is first 50% from 0 to 127 and the next 50% from 127 to 0
* `CC 83 [x]` &mdash; save settings to EEPROM. The parameter `x` can be anything. Responds with same message.

## License
See the `LICENSE` file (MIT license).