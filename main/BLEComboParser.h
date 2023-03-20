#ifndef BLECOMBOPARSER_H
#define BLECOMBOPARSER_H

#include <BLECombo.h>

class BLEComboParser : public BLECombo {
public:
    BLEComboParser(std::string deviceName = "ESP32 Combo", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 90);
    size_t parseHIDDataKeyboard(uint8_t* buf);
    size_t parseHIDDataMouse(uint8_t* buf);
};

#endif
