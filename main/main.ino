#include "DebugPrint.h"
#include "USBBLERelay.h"

/*
Real    Divided  ADC
4.2V -> 2.8V  -> 3475
3.6V -> 2.4V  -> 2978
*/
#define MONITOR_PIN_LED 22
#define MONITOR_PIN_BATTERY_VOLT 4
#define MONITOR_BATTERY_VOLT_MAX 3475
#define MONITOR_BATTERY_VOLT_MIN 2978
#define MONITOR_BLINK_FREQ 1000
#define MONITOR_UPDATE_FREQ 1000000

USBBLERelay relay;

void setup()
{
    pinMode(MONITOR_PIN_LED, OUTPUT);
    pinMode(MONITOR_PIN_BATTERY_VOLT, INPUT);
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
    static int update_counter;
    relay.task();
    if (relay._bleCombo.isConnected()) {
        digitalWrite(MONITOR_PIN_LED, LOW);
        if (update_counter % MONITOR_UPDATE_FREQ == 0) {
            int volt_level = analogRead(MONITOR_PIN_BATTERY_VOLT);
            DEBUG_PRINTF("Voltage level %d\n", volt_level);
            volt_level = map(volt_level, MONITOR_BATTERY_VOLT_MIN, MONITOR_BATTERY_VOLT_MAX, 0, 100);
            if (volt_level < 0)
                volt_level = 0;
            if (volt_level > 100)
                volt_level = 100;
            DEBUG_PRINTF("Voltage level %d\n", volt_level);
            relay._bleCombo.setBatteryLevel(volt_level);
        }
        update_counter++;
    } else {
        long blink_stat = millis() / MONITOR_BLINK_FREQ;
        if (blink_stat % 2)
            digitalWrite(MONITOR_PIN_LED, HIGH);
        else
            digitalWrite(MONITOR_PIN_LED, LOW);
    }
}
