# MidiWah v1.0
Arduino based bridge between a wah-wah (e.g. Dunlop GCB-95) and its potentiometer. Digitizes the potentiometer and feeds it via a digital potentiometer back to the circuit.

As a bonus, the digitized potentiometer signal can be  outputted via MIDI OUT.

## Contents
* Eagle schematic and PCB
* Arduino firmware

## Eagle

### Bill of materials
An Atmega328 can also be used. The R7-R9 are 0-ohm jumpers. The MCP4251 has a range of 0-100k.

| Part | Value     | Device          | Package           |
|------|-----------|-----------------|-------------------|
| C1   | 22pF      | C-EU025-024X044 | C025-024X044      |
| C2   | 22pF      | C-EU025-024X044 | C025-024X044      |
| C3   | 1µF       | CPOL-EUE2.5-5   | E2,5-5            |
| C4   | 1µF       | CPOL-EUE2.5-5   | E2,5-5            |
| C5   | 100nF     | C-EU025-024X044 | C025-024X044      |
| C6   | 100nF     | C-EU025-024X044 | C025-024X044      |
| C7   | 100nF     | C-EU025-024X044 | C025-024X044      |
| C8   | 100nF     | C-EU025-024X044 | C025-024X044      |
| D1   | 1N4148    | 1N4148          | DO35-7            |
| IC1  | ATMEGA168 | MEGA8-P         | DIL28-3           |
| IC2  | 7805L     | 7805L           | TO92              |
| IC3  | MCP4251   | MCP4251-104     | DIP254P762X533-14 |
| IC4  | 6N138     | 6N138           | DIL08             |
| Q1   | 16MHz     | CRYSTALHC49S    | HC49/S            |
| R1   | 10k       | R-EU_0204/7     | 0204/7            |
| R2   | 220       | R-EU_0204/7     | 0204/7            |
| R3   | 1k        | R-EU_0204/7     | 0204/7            |
| R4   | 220       | R-EU_0204/7     | 0204/7            |
| R5   | 1k        | R-EU_0204/7     | 0204/7            |
| R6   | 1k        | R-EU_0204/2V    | 0204V             |
| R7   | 0         | R-EU_M0805      | M0805             |
| R8   | 0         | R-EU_M0805      | M0805             |
| R9   | 0         | R-EU_M0805      | M0805             |
| SW1  |           | SW_DIP-4        | EDG-04            |

### Board render
The board measures 1.70x2.10 inch. Below a render of the top and bottom.

![brd](https://raw.githubusercontent.com/basilfx/Arduino-MidiWah/master/docs/board.png)

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