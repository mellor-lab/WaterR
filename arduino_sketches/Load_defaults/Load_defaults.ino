#include <EEPROM.h>

void setup() {
  for (int i=0; i<4; i++) {     //default timers at zero
    EEPROM.write(i, 0);
  }

  EEPROM.write(4, 0);           //default volume at zero
  EEPROM.write(5, 0);           //default last priming day at zero
  
  EEPROM.write(6, 0);           //default piezo sensitivity threshold at 200;
  EEPROM.write(7, 200);
}

void loop() {
}
