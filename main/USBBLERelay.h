#ifndef USBBLERELAY_H
#define USBBLERELAY_H

#include "HIDSelector.h"
#include "BLEComboParser.h"
#include <hidcomposite.h>
#include <usbhub.h>

class USBBLERelay {
public:
    USBBLERelay(void);
    void begin(void);
    void task(void);

    USB _usb;
    USBHub _hub;
    HIDSelector _hidSelector;
    BLEComboParser _bleCombo;
};

#endif
