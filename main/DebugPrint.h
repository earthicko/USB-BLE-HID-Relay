#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include <Arduino.h>
#include "SerialPrintf.h"

// #define _DEBUG

#ifdef _DEBUG
#define DEBUG_PRINT(str) Serial.print(str)
#define DEBUG_PRINTF(...) SerialPrintf(__VA_ARGS__)
#define DEBUG_PRINTLN(str) Serial.println(str)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x, ...)
#define DEBUG_PRINTLN(x)
#endif

#endif
