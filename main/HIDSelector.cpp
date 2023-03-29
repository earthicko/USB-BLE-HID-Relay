#include "HIDSelector.h"
#include "DebugPrint.h"
#include "MsgPipe.h"
#include "remapKeyboard.h"

HIDSelector::HIDSelector(USB* p, MsgPipe<hidmsg_t>* hidpipe)
    : HIDComposite(p)
    , _hidpipe(hidpipe)
{
}

// Return true for the interface we want to hook into
bool HIDSelector::SelectInterface(uint8_t iface, uint8_t proto)
{
    return (proto != 0);
}

static void turnoffAllOutput(MsgPipe<hidmsg_t>* pipe)
{
    hidmsg_t msg;
    msg.ep = 1;
    msg.len = 8;
    for (uint8_t i = 0; i < msg.len; i++)
        msg.buf[i] = 0;
    pipe->push(&msg);
    msg.ep = 2;
    msg.len = 8;
    for (uint8_t i = 0; i < msg.len; i++)
        msg.buf[i] = 0;
    pipe->push(&msg);
}

// Will be called for all HID data received from the USB interface
void HIDSelector::ParseHIDData(USBHID* hid, uint8_t ep, bool is_rpt_id, uint8_t len, uint8_t* buf)
{
    DEBUG_PRINTF("HID %p, ep %d, is rpt %d, len %d, buf {", hid, ep, is_rpt_id, len);
    for (uint8_t i = 0; i < len; i++)
        DEBUG_PRINTF("%d, ", (int8_t)buf[i]);
    DEBUG_PRINTF("}, %llu", *((uint64_t*)buf));
    DEBUG_PRINTF("\n");

    if (ep == 2 && *((uint32_t*)buf) == 0x1000003) {
        // buf = {3, 0, 0, 1, } (Fn + Space)
        turnoffAllOutput(_hidpipe);
        uint8_t lockLeds = 1;
        SetReport(0, 0, 2, 0, 1, &lockLeds);
        delay(200);
        lockLeds = 2;
        SetReport(0, 0, 2, 0, 1, &lockLeds);
        delay(200);
        lockLeds = 4;
        SetReport(0, 0, 2, 0, 1, &lockLeds);
        delay(200);
        lockLeds = 0;
        SetReport(0, 0, 2, 0, 1, &lockLeds);
        DEBUG_PRINTF("Reset Controller\n");
        ESP.restart();
        return;
    }

    if (ep == 1) {
        uint8_t modifierRemapped = 0;
        for (int i = 0; i < 8; i++) {
            if (buf[0] & keycodeMapModifier[i][0])
                modifierRemapped |= keycodeMapModifier[i][1];
        }
        buf[0] = modifierRemapped;
        for (int i = 2; i < 8; i++)
            buf[i] = keycodeMap[buf[i]];
    }

    hidmsg_t msg;
    msg.ep = ep;
    msg.len = len;
    for (uint8_t i = 0; i < len; i++)
        msg.buf[i] = buf[i];
    _hidpipe->push(&msg);
}
