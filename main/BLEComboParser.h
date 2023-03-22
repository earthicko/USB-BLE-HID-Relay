#ifndef BLECOMBOPARSER_H
#define BLECOMBOPARSER_H

#include <BLECombo.h>

#define BLECOMBOPARSER_MOVE_MULTIPLIER 3
#define BLECOMBOPARSER_MOVE_DIVIDER 2
#define BLECOMBOPARSER_SCROLL_MULTIPLIER 1
#define BLECOMBOPARSER_SCROLL_DIVIDER 2
#define BLECOMBOPARSER_SCROLL_UPDATE_FREQ 5

class BLEComboParser : public BLECombo {
public:
    BLEComboParser(std::string deviceName = "ESP32 Combo", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 90);
    size_t parseHIDDataKeyboard(int8_t* buf);
    size_t parseHIDDataMouse(int8_t* buf);
};

typedef size_t (BLEComboParser::*parser_t)(int8_t*);

#endif
