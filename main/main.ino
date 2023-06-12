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
#define MONITOR_BLINK_PERIOD 1000
#define MONITOR_UPDATE_PERIOD_BATT 30000
#define MONITOR_UPDATE_PERIOD_LED 300

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

void loop()
{
    static bool connection;

    relay.task();
    if (relay._bleCombo.isConnected()) {
        if (connection == false) {
            DEBUG_PRINTF("Connected\n");
            uint8_t lockLeds = relay._bleCombo.getKeyLedValue();
            relay._hidSelector.SetReport(0, 0, 2, 0, 1, &lockLeds);
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
            relay._bleCombo.setBatteryLevel(volt_level);
        }
    } else {
        if (connection == true)
            connection = false;
        if (should_update_blink_led()) {
            long blink_stat = millis() / MONITOR_BLINK_PERIOD;
            uint8_t lockLeds;
            if (blink_stat % 2) {
                digitalWrite(MONITOR_PIN_LED, HIGH);
                lockLeds = 0;
            } else {
                digitalWrite(MONITOR_PIN_LED, LOW);
                lockLeds = 7;
            }
            relay._hidSelector.SetReport(0, 0, 2, 0, 1, &lockLeds);
        }
    }
}
