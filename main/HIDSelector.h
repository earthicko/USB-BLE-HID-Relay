#ifndef HIDSELECTOR_H
#define HIDSELECTOR_H

#include "BLEComboParser.h"
#include <hidcomposite.h>
#include <usbhub.h>

// Override HIDComposite to be able to select which interface we want to hook into
class HIDSelector : public HIDComposite {
public:
    HIDSelector(USB* p, BLEComboParser* ble);

protected:
    void ParseHIDData(USBHID* hid, uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t* buf); // Called by the HIDComposite library
    bool SelectInterface(uint8_t iface, uint8_t proto);

private:
    BLEComboParser* _ble;
};

#endif
