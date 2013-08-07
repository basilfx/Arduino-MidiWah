#include <SPI.h>
#include <MIDI.h>
#include <EEPROM.h>
#include <McpDigitalPot.h>

#ifdef DEBUG
#include <SoftwareSerial.h>
#endif

#define SWITCH_1 5
#define SWITCH_2 6
#define SWITCH_3 7
#define SWITCH_4 8

#define MODE 3
#define SOFT_TX 2
#define SOFT_RX 9

#define POT_SS 10
#define WAH_IN 0

#define ADC_READINGS 10

struct AdcInfo {
  unsigned int readings[ADC_READINGS];
  unsigned int index;
  unsigned int total;
  unsigned int average;
};

struct Settings {
  unsigned int checksum;
  unsigned int adcMin;
  unsigned int adcMax;
  unsigned char program;
  boolean calibrate;
  boolean invert;
};

#ifdef DEBUG
SoftwareSerial Debug(SOFT_RX, SOFT_TX);
#endif
McpDigitalPot Potmeter = McpDigitalPot(POT_SS, 10000);

unsigned char lastChannel = 0;
unsigned char lastWah = 0;

AdcInfo adcWah;
Settings settings;

#define MAX_PROGRAM 3

unsigned char program1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 33, 34, 35, 37, 38, 40, 41, 43, 45, 46, 48, 50, 52, 54, 56, 59, 61, 63, 66, 68, 71, 74, 77, 80, 83, 86, 90, 93, 97, 101, 105, 109, 113, 118, 122, 127};
unsigned char program2[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 126, 124, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0};

void setup() {
  // Setup debug output
  #ifdef DEBUG
  Debug.begin(38400);
  Debug.println("Debug ready.");
  #endif

  // Load config
  loadConfig();
  
  // Configure pins
  pinMode(MODE, INPUT);  
  pinMode(SWITCH_1, INPUT);
  pinMode(SWITCH_2, INPUT);
  pinMode(SWITCH_3, INPUT);
  pinMode(SWITCH_4, INPUT);

  digitalWrite(MODE, HIGH);  
  digitalWrite(SWITCH_1, HIGH);
  digitalWrite(SWITCH_2, HIGH);
  digitalWrite(SWITCH_3, HIGH);
  digitalWrite(SWITCH_4, HIGH);
  
  // Setup Potmeter
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  Potmeter.scale = 127.0;
  
  // Setup MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Initialize ADC
  adcWah.average = 0;
  adcWah.total = 0;
  
  for (int i = 0; i < ADC_READINGS; i++) {
    adcWah.readings[i] = 0;
  }
}

void loop() {
  // Check for changed channel number
  unsigned char channel = getChannel();
  
  if (lastChannel != channel) {
    #ifdef DEBUG
    Debug.print("Channel changed to ");
    Debug.print(channel);
    Debug.println(".");
    lastChannel = channel;
    #endif

    MIDI.setInputChannel(channel);
  }
  
  // Handle MIDI-in
  if (MIDI.read()) {
    unsigned char type = MIDI.getType();
    
    if (type == ControlChange) {
      unsigned char control = MIDI.getData1();
      unsigned char value = MIDI.getData2();
 
      #ifdef DEBUG
      Debug.print("Control changed to ");
      Debug.print(control, HEX);
      Debug.print(" with value ");
      Debug.print(value, DEC);
      Debug.println(".");
      #endif

      switch (control) {
        case 0x43: // External control
          break;
        case 0x50: // Calibrate
          if (value == 1) {
            settings.calibrate = true;
            
            settings.adcMin = 512;
            settings.adcMax = 512;
            
            #ifdef DEBUG
            Debug.println("Calibrate is on."); 
            #endif

            // Send ack            
            MIDI.sendControlChange(0x50, 1, channel);
          } else {
            settings.calibrate = false;
            #ifdef DEBUG
            Debug.println("Calibrate is off.");
            #endif
          }

          // Send ack            
          MIDI.sendControlChange(0x50, 0, channel);
          
          break;
        case 0x51: // Invert
          if (value == 1) {
            settings.invert = true;
            
            #ifdef DEBUG
            Debug.println("Invert is on."); 
            #endif
            
            // Send ack            
            MIDI.sendControlChange(0x51, 1, channel);
          } else {
            settings.invert = false;
            
            #ifdef DEBUG
            Debug.println("Invert is off.");
            #endif
            
            // Send ack            
            MIDI.sendControlChange(0x51, 0, channel);
          }
          
          break;
        case 0x52: // Program
          if (value < MAX_PROGRAM) {
            settings.program = value;
            
            #ifdef DEBUG
            Debug.print("Program ");
            Debug.print(value, DEC);
            Debug.println(" selected.");
            #endif
          } else {
            #ifdef DEBUG
            Debug.print("Invalid program. Max program is ");
            Debug.print(MAX_PROGRAM, DEC);
            Debug.println(".");
            #endif
          }
          
          // Send ack            
          MIDI.sendControlChange(0x52, settings.program, channel);
          
          break;
        case 0x53: // Save
          saveConfig();
          
          // Send ack            
          MIDI.sendControlChange(0x53, 1, channel);

          break;
        default:
          break;
      }
    }
  }
 
  // Read new ADC value
  unsigned int wah = getWah();
  
  if (lastWah != wah) {
    #ifdef DEBUG
    Debug.print("Wah changed to ");
    Debug.print(wah, DEC);
    Debug.println(".");
    #endif
    
    lastWah = wah;
    
    // Output to MIDI
    MIDI.sendControlChange(0x43, wah, channel);
    
    // Update resistance
    Potmeter.setResistance(1, wah);
  }
}

void loadConfig() {
  for (unsigned int i = 0; i < sizeof(settings); i++) {
    *((char *) &settings + i) = EEPROM.read(i);
  }
  
  // Check for valid settings and if not, use defaults
  if (settings.checksum != 0xAAAA) {
    #ifdef DEBUG
    Debug.println("Settings checksum invalid. Using defaults.");
    #endif
    
    settings.checksum = 0xAAAA;
    settings.adcMin = 0;
    settings.adcMax = 1023;
    settings.calibrate = true;
    settings.invert = false;
    settings.program = 0;
    
    saveConfig();
  }

  #ifdef DEBUG
  Debug.println("Settings loaded from EEPROM.");  
  #endif
}

void saveConfig() {
  for (unsigned int i = 0; i < sizeof(settings); i++) {
    EEPROM.write(i, *((char*) &settings + i));
  } 
  
  #ifdef DEBUG
  Debug.println("Settings saved to EEPROM.");
  #endif
}

unsigned char getChannel() {
  unsigned char channel = 0x00;
 
  channel += !digitalRead(SWITCH_1) << 3;
  channel += !digitalRead(SWITCH_2) << 2;
  channel += !digitalRead(SWITCH_3) << 1;
  channel += !digitalRead(SWITCH_4) << 0;
  
  return channel + 1;
}

unsigned char getWah() {
  unsigned int reading = analogRead(WAH_IN);
  
  if (settings.calibrate) {
    if (reading > settings.adcMax) {
      settings.adcMax = reading;
    } else if (reading < settings.adcMin) {
      settings.adcMin = reading; 
    }
  }
  
  adcWah.total = adcWah.total - adcWah.readings[adcWah.index];
  adcWah.readings[adcWah.index] = reading;
  adcWah.total = adcWah.total + adcWah.readings[adcWah.index];
  adcWah.index = (adcWah.index + 1) % ADC_READINGS;
  adcWah.average = adcWah.total / ADC_READINGS;
  
  unsigned char result = map(adcWah.average, settings.adcMin, settings.adcMax, 0, 127);
  
  if (settings.program == 1) {
    result = program1[result];
  } else if (settings.program == 2) {
    result = program2[result];
  }
  
  if (settings.invert) {
    return result;
  } else {
    return 127 - result;
  }
}
