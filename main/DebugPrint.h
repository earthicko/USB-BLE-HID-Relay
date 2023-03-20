#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include <Arduino.h>
#include "SerialPrintf.h"

#ifdef _DEBUG
#define DEBUG_PRINT(str) Serial.print(str)
#define DEBUG_PRINTF(str, ...) SerialPrintf(str, ...)
#define DEBUG_PRINTLN(str) Serial.println(str)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x, ...)
#define DEBUG_PRINTLN(x)
#endif

#endif
