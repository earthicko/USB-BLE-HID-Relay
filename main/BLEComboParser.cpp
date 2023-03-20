#include "BLEComboParser.h"

BLEComboParser::BLEComboParser(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel)
    : BLECombo(deviceName, deviceManufacturer, batteryLevel)
{
}

size_t BLEComboParser::parseHIDDataKeyboard(uint8_t* buf)
{
    this->sendReport((keyreport_t*)buf);
    return (1);
}

size_t BLEComboParser::parseHIDDataMouse(uint8_t* buf)
{
    if (buf[0] == 1) {
        uint8_t m[5];
        m[0] = buf[1]; // button state
        m[1] = buf[2]; // dx
        m[2] = buf[3]; // dy
        m[3] = 0; // vertical wheel
        m[4] = 0; // horizontal wheel
        mouseNotify(sizeof(m), m);
    } else if (buf[0] == 3) {
        if (buf[1] == 0 && buf[2] == 0 && buf[3] == 0) { // release
            keyRelease(KEY_MEDIA_WWW_HOME);
            keyRelease(KEY_MEDIA_VOLUME_UP);
            keyRelease(KEY_MEDIA_VOLUME_DOWN);
            keyRelease(KEY_MEDIA_MUTE);
        } else if (buf[1] == 0 && buf[2] == 64 && buf[3] == 0) { // ACCESS_IBM
            keyPress(KEY_MEDIA_WWW_HOME);
        } else if (buf[1] == 1 && buf[2] == 0 && buf[3] == 0) {
            keyPress(KEY_MEDIA_VOLUME_UP);
        } else if (buf[1] == 2 && buf[2] == 0 && buf[3] == 0) {
            keyPress(KEY_MEDIA_VOLUME_DOWN);
        } else if (buf[1] == 4 && buf[2] == 0 && buf[3] == 0) {
            keyPress(KEY_MEDIA_MUTE);
        }
    }
    return (0);
}
