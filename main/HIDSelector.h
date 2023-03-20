#ifndef HIDSELECTOR_H
#define HIDSELECTOR_H

#include "BLEComboParser.h"
#include <hidcomposite.h>
#include <usbhub.h>

#define UHS_HID_BOOT_KEY_CAPS_LOCK 0x39
#define UHS_HID_BOOT_KEY_SCROLL_LOCK 0x47
#define UHS_HID_BOOT_KEY_NUM_LOCK 0x53

struct KBDLEDS {
    uint8_t bmNumLock : 1;
    uint8_t bmCapsLock : 1;
    uint8_t bmScrollLock : 1;
    uint8_t bmCompose : 1;
    uint8_t bmKana : 1;
    uint8_t bmReserved : 3;
};

// Override HIDComposite to be able to select which interface we want to hook into
class HIDSelector : public HIDComposite {
public:
    HIDSelector(USB* p, BLEComboParser* ble);

protected:
    void ParseHIDData(USBHID* hid, uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t* buf); // Called by the HIDComposite library
    bool SelectInterface(uint8_t iface, uint8_t proto);
    uint8_t HandleLockingKeys(USBHID* hid, uint8_t key);

    union {
        KBDLEDS kbdLeds;
        uint8_t bLeds;
    } kbdLockingKeys;

private:
    BLEComboParser* _ble;
};

#endif
