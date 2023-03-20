#include "DebugPrint.h"
#include "USBBLERelay.h"

USBBLERelay relay;

void setup()
{
#ifdef _DEBUG
    Serial.begin(115200);
#endif
    DEBUG_PRINTF("Start");
    // Set this to higher values to enable more debug information
    // minimum 0x00, maximum 0xff, default 0x80
    UsbDEBUGlvl = 0x00;
    relay.begin();
}

void loop()
{
    relay.task();
}
