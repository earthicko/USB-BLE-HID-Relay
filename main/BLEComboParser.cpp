#include "BLEComboParser.h"
#include "DebugPrint.h"

BLEComboParser::BLEComboParser(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel)
    : BLECombo(deviceName, deviceManufacturer, batteryLevel)
{
}

void BLEComboParser::parseHIDDataKeyboard(const int8_t* buf)
{
    this->sendReport((keyreport_t*)buf);
}

static bool should_update_scroll(void)
{
    static int counter;

    counter++;
    if (counter != BLECOMBOPARSER_SCROLL_UPDATE_FREQ)
        return (false);
    counter = 0;
    return (true);
}

void BLEComboParser::parseHIDDataMouse(const int8_t* buf)
{
    static bool prev_middle_stat;
    static bool did_trigger_wheel_movement;
    static int scroll_d[2];

    if (buf[0] == 1) {
        int8_t m[5];
        if (buf[1] & MOUSE_MIDDLE) {
            // middle button stays being pressed
            if (should_update_scroll()) {
                // update scroll movement
                m[0] = buf[1] & ~((int8_t)MOUSE_MIDDLE); // button state
                m[1] = 0; // dx
                m[2] = 0; // dy
                m[3] = scroll_d[0] * BLECOMBOPARSER_SCROLL_MULTIPLIER / BLECOMBOPARSER_SCROLL_DIVIDER / BLECOMBOPARSER_SCROLL_UPDATE_FREQ; // vertical wheel = dy
                m[4] = scroll_d[1] * BLECOMBOPARSER_SCROLL_MULTIPLIER / BLECOMBOPARSER_SCROLL_DIVIDER / BLECOMBOPARSER_SCROLL_UPDATE_FREQ; // horizontal wheel = dx
                did_trigger_wheel_movement = true;
                scroll_d[0] = 0;
                scroll_d[1] = 1;
            } else {
                // ignore scroll movement
                m[0] = buf[1] & ~((int8_t)MOUSE_MIDDLE); // button state
                m[1] = 0; // dx
                m[2] = 0; // dy
                m[3] = 0;
                m[4] = 0;
                scroll_d[0] += -buf[3];
                scroll_d[1] += buf[2];
            }
        } else if (!(buf[1] & MOUSE_MIDDLE) && (prev_middle_stat)) {
            // middle button just being released
            if (did_trigger_wheel_movement) {
                // middle button was pressed to scroll
                m[0] = buf[1]; // button state
                m[1] = ((int)(buf[2])) * BLECOMBOPARSER_MOVE_MULTIPLIER / BLECOMBOPARSER_MOVE_DIVIDER; // dx
                m[2] = ((int)(buf[3])) * BLECOMBOPARSER_MOVE_MULTIPLIER / BLECOMBOPARSER_MOVE_DIVIDER; // dy
                m[3] = 0; // vertical wheel
                m[4] = 0; // horizontal wheel
                did_trigger_wheel_movement = false;
            } else {
                // middle button was pressed to act as wheel click
                mouseClick(MOUSE_MIDDLE);
                prev_middle_stat = false;
                return;
            }
        } else {
            // middle button stays being released
            m[0] = buf[1]; // button state
            m[1] = ((int)(buf[2])) * BLECOMBOPARSER_MOVE_MULTIPLIER / BLECOMBOPARSER_MOVE_DIVIDER; // dx
            m[2] = ((int)(buf[3])) * BLECOMBOPARSER_MOVE_MULTIPLIER / BLECOMBOPARSER_MOVE_DIVIDER; // dy
            m[3] = 0; // vertical wheel
            m[4] = 0; // horizontal wheel
        }
        prev_middle_stat = buf[1] & MOUSE_MIDDLE;

        DEBUG_PRINTF("m = {");
        for (int i = 0; i < 5; i++)
            DEBUG_PRINTF("%d, ", m[i]);
        DEBUG_PRINTF("}\n");

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
}
