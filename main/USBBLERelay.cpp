#include "USBBLERelay.h"
#include "DebugPrint.h"

USBBLERelay::USBBLERelay(void)
    : _usb()
    , _hub(&_usb)
    , _bleCombo("IBM UltraNav", "IBM / rokart", 100)
    , _hidSelector(&_usb, &_bleCombo)
{
}

void USBBLERelay::begin(void)
{
    _bleCombo.begin();
    DEBUG_PRINTF("Starting BLE work!");
    if (_usb.Init() == -1)
        DEBUG_PRINTF("OSC did not start.");
    delay(200);
}

void USBBLERelay::task(void)
{
    _usb.Task();
}
