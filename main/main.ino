#include "BLEComboParser.h"
#include "DebugPrint.h"
#include "HIDSelector.h"
#include "MsgPipe.h"
#include <hidcomposite.h>
#include <usbhub.h>

/*
Real    Divided  ADC
4.2V -> 2.8V  -> 3475
3.6V -> 2.4V  -> 2978
*/
#define MONITOR_PIN_LED 22
#define MONITOR_PIN_BATTERY_VOLT 4
#define MONITOR_BATTERY_VOLT_MAX 3475
#define MONITOR_BATTERY_VOLT_MIN 2978
#define MONITOR_BLINK_PERIOD 1000
#define MONITOR_UPDATE_PERIOD_BATT 30000
#define MONITOR_UPDATE_PERIOD_LED 300

TaskHandle_t BLETaskHandle;
MsgPipe<hidmsg_t> hidmsgPipe;
MsgPipe<uint8_t> ledstatPipe;
USB usb;
USBHub hub(&usb);
HIDSelector hidSelector(&usb, &hidmsgPipe);
BLEComboParser ble("IBM UltraNav", "IBM / rokart", 100);

static bool should_update_blink_led(void)
{
    static int prev_update_time;

    int time_now = millis() / MONITOR_BLINK_PERIOD;
    if (time_now != prev_update_time) {
        prev_update_time = time_now;
        return (true);
    }
    return (false);
}

static bool should_update_battery(void)
{
    static int prev_update_time;

    int time_now = millis() / MONITOR_UPDATE_PERIOD_BATT;
    if (time_now != prev_update_time) {
        prev_update_time = time_now;
        return (true);
    }
    return (false);
}

static bool should_update_led(void)
{
    static int prev_update_time;

    int time_now = millis() / MONITOR_UPDATE_PERIOD_LED;
    if (time_now != prev_update_time) {
        prev_update_time = time_now;
        return (true);
    }
    return (false);
}

void BLETask(void* parameter) // Core 0 (BLE)
{
    static bool connection;
    static const parser_t parsers[] = {
        (const parser_t)(NULL),
        (const parser_t)(&BLEComboParser::parseHIDDataKeyboard),
        (const parser_t)(&BLEComboParser::parseHIDDataMouse),
    };
    static uint8_t lockLedsPrev;
    uint8_t lockLeds;

    DEBUG_PRINTF("BLETask start.\n");
    ble.begin();
    while (true) {
        if (ble.isConnected()) {
            if (connection == false) {
                DEBUG_PRINTF("BLE Connected\n");
                lockLeds = ble.getKeyLedValue();
                connection = true;
            }
            digitalWrite(MONITOR_PIN_LED, LOW);
            if (should_update_battery()) {
                int volt_level = analogRead(MONITOR_PIN_BATTERY_VOLT);
                DEBUG_PRINTF("Voltage level %d ", volt_level);
                volt_level = map(volt_level, MONITOR_BATTERY_VOLT_MIN, MONITOR_BATTERY_VOLT_MAX, 0, 100);
                if (volt_level < 0)
                    volt_level = 0;
                if (volt_level > 100)
                    volt_level = 100;
                DEBUG_PRINTF("Battery %d%%\n", volt_level);
                ble.setBatteryLevel(volt_level);
            }
        } else {
            if (connection == true) {
                DEBUG_PRINTF("BLE Disconnected\n");
                connection = false;
            }
            if (should_update_blink_led()) {
                long blink_stat = millis() / MONITOR_BLINK_PERIOD;
                if (blink_stat % 2) {
                    digitalWrite(MONITOR_PIN_LED, HIGH);
                    lockLeds = 0;
                } else {
                    digitalWrite(MONITOR_PIN_LED, LOW);
                    lockLeds = 7;
                }
                ledstatPipe.push(&lockLeds);
            }
        }
        hidmsg_t msg;
        if (hidmsgPipe.pop(&msg)) {
            vTaskDelay(1);
            continue;
        }
        if (!ble.isConnected()) {
            vTaskDelay(1);
            continue;
        }
        (ble.*parsers[msg.ep])((int8_t*)msg.buf);
        if (msg.ep == 1 && *((uint64_t*)msg.buf) == 0)
            lockLeds = ble.getKeyLedValue();

        if (lockLeds != lockLedsPrev) {
            ledstatPipe.push(&lockLeds);
            DEBUG_PRINTF("LED Status updated to %#x\n", lockLeds);
            lockLedsPrev = lockLeds;
        }
    }
}

void setup()
{
#ifdef _DEBUG
    Serial.begin(115200);
#endif
    DEBUG_PRINTF("Start\n");
    xTaskCreatePinnedToCore(
        BLETask, // Function to implement the task
        "BLETask", // Name of the task
        10000, // Stack size in words
        NULL, // Task input parameter
        1, // Priority of the task
        &BLETaskHandle, // Task handle.
        0); // Core where the task should run
    DEBUG_PRINTF("USBTask start.\n");
    if (usb.Init() == -1)
        DEBUG_PRINTF("OSC did not start.\n");
    else
        DEBUG_PRINTF("OSC start.\n");
}

void loop()
{
    usb.Task();
    uint8_t lockLeds;
    if (ledstatPipe.pop(&lockLeds) == 0)
        hidSelector.SetReport(0, 0, 2, 0, 1, &lockLeds);
}
