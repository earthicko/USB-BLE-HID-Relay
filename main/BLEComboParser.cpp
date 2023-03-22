#include "BLEComboParser.h"

BLEComboParser::BLEComboParser(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel)
    : BLECombo(deviceName, deviceManufacturer, batteryLevel)
{
}

size_t BLEComboParser::parseHIDDataKeyboard(int8_t* buf)
{
    this->sendReport((keyreport_t*)buf);
    return (1);
}

size_t BLEComboParser::parseHIDDataMouse(int8_t* buf)
{
    static int8_t wheel_update_counter;

    if (buf[0] == 1) {
        int8_t m[5];
        if (buf[1] & MOUSE_MIDDLE && (buf[2] || buf[3])) {
            wheel_update_counter++;
            if (wheel_update_counter != BLECOMBOPARSER_SCROLL_UPDATE_FREQ)
                return (0);
            m[0] = buf[1] & ~((int8_t)MOUSE_MIDDLE); // button state
            m[1] = 0; // dx
            m[2] = 0; // dy
            m[3] = ((int)(-buf[3])) * BLECOMBOPARSER_SCROLL_MULTIPLIER / BLECOMBOPARSER_SCROLL_DIVIDER; // vertical wheel = dy
            m[4] = ((int)(buf[2])) * BLECOMBOPARSER_SCROLL_MULTIPLIER / BLECOMBOPARSER_SCROLL_DIVIDER; // horizontal wheel = dx
            if (wheel_update_counter == BLECOMBOPARSER_SCROLL_UPDATE_FREQ)
                wheel_update_counter = 0;
        } else {
            m[0] = buf[1]; // button state
            m[1] = ((int)(buf[2])) * BLECOMBOPARSER_MOVE_MULTIPLIER / BLECOMBOPARSER_MOVE_DIVIDER; // dx
            m[2] = ((int)(buf[3])) * BLECOMBOPARSER_MOVE_MULTIPLIER / BLECOMBOPARSER_MOVE_DIVIDER; // dy
            m[3] = 0; // vertical wheel
            m[4] = 0; // horizontal wheel
        }
        mouseNotify(sizeof(m), (uint8_t*)m);
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
