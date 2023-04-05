#include "HIDSelector.h"
#include "DebugPrint.h"

HIDSelector::HIDSelector(USB* p, BLEComboParser* ble)
    : HIDComposite(p)
    , _ble(ble) {};

// Return true for the interface we want to hook into
bool HIDSelector::SelectInterface(uint8_t iface, uint8_t proto)
{
    return (proto != 0);
}

// Will be called for all HID data received from the USB interface
void HIDSelector::ParseHIDData(USBHID* hid, uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t* buf)
{
    static const parser_t parsers[] = {
        (const parser_t)(NULL),
        (const parser_t)(&BLEComboParser::parseHIDDataKeyboard),
        (const parser_t)(&BLEComboParser::parseHIDDataMouse),
    };
    static uint8_t prevLockLeds;

    DEBUG_PRINTF("HID %p, ep %d, is rpt %d, len %d, buf {", hid, ep, is_rpt_id, len);
    for (uint8_t i = 0; i < len; i++)
        DEBUG_PRINTF("%d, ", (int8_t)buf[i]);
    DEBUG_PRINTF("}, %llu", *((uint64_t*)buf));
    DEBUG_PRINTF("\n");

    if (ep == 2 && *((uint32_t*)buf) == 0x1000003) {
        // buf = {3, 0, 0, 1, } (Fn + Space)
        uint64_t empty;
        _ble->parseHIDDataKeyboard((int8_t*)&empty);
        _ble->parseHIDDataMouse((int8_t*)&empty);
        uint8_t lockLeds = 7;
        SetReport(0, 0, 2, 0, 1, &lockLeds);
        delay(100);
        DEBUG_PRINTF("Reset Controller\n");
        ESP.restart();
        return;
    }

    if (!(_ble->isConnected()))
        return;
    (_ble->*parsers[ep])((int8_t*)buf);
    if (ep == 1) {
        if (*((uint64_t*)buf) == 0) {
            uint8_t lockLeds = _ble->getKeyLedValue();
            if (lockLeds != prevLockLeds) {
                hid->SetReport(0, 0, 2, 0, 1, &lockLeds);
                prevLockLeds = lockLeds;
            }
        }
    }
}
