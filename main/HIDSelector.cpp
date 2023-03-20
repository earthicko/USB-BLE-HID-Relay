#include "HIDSelector.h"
#include "DebugPrint.h"

HIDSelector::HIDSelector(USB* p, BLEComboParser* ble)
    : HIDComposite(p)
    , _ble(ble)
{
    kbdLockingKeys.bLeds = 0;
};

// Return true for the interface we want to hook into
bool HIDSelector::SelectInterface(uint8_t iface, uint8_t proto)
{
    return (proto != 0);
}

// Will be called for all HID data received from the USB interface
void HIDSelector::ParseHIDData(USBHID* hid, uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t* buf)
{
    DEBUG_PRINTF("HID %p, ep %d, is rpt %d, len %d, buf {", hid, ep, is_rpt_id, len);
    for (uint8_t i = 0; i < len; i++)
        DEBUG_PRINTF("%d, ", (int8_t)buf[i]);
    DEBUG_PRINTF("}, %llu", *((uint64_t*)buf));
    DEBUG_PRINTF("\n");
    if (!(_ble->isConnected()))
        return;
    static const parser_t parsers[] = {
        (const parser_t)(NULL),
        (const parser_t)(&BLEComboParser::parseHIDDataKeyboard),
        (const parser_t)(&BLEComboParser::parseHIDDataMouse),
    };
    (_ble->*parsers[ep])(buf);
}

uint8_t HIDSelector::HandleLockingKeys(USBHID* hid, uint8_t key)
{
    uint8_t old_keys = kbdLockingKeys.bLeds;
    switch (key) {
    case UHS_HID_BOOT_KEY_NUM_LOCK:
        kbdLockingKeys.kbdLeds.bmNumLock = ~kbdLockingKeys.kbdLeds.bmNumLock;
        break;
    case UHS_HID_BOOT_KEY_CAPS_LOCK:
        kbdLockingKeys.kbdLeds.bmCapsLock = ~kbdLockingKeys.kbdLeds.bmCapsLock;
        break;
    case UHS_HID_BOOT_KEY_SCROLL_LOCK:
        kbdLockingKeys.kbdLeds.bmScrollLock = ~kbdLockingKeys.kbdLeds.bmScrollLock;
        break;
    }
    if (old_keys != kbdLockingKeys.bLeds && hid) {
        uint8_t lockLeds = kbdLockingKeys.bLeds;
        return (hid->SetReport(0, 0 /*hid->GetIface()*/, 2, 0, 1, &lockLeds));
    }
    return 0;
};
