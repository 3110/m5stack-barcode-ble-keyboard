#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <esp_log.h>

#define XSTR(x) #x
#define STR(x)  XSTR(x)

class M5UnitQRCode {
public:
    M5UnitQRCode(void) {
#if defined(LOG_LOCAL_LEVEL)
        esp_log_level_set(STR(ESP_LOG_TAG), LOG_LOCAL_LEVEL);
#endif
    }
    virtual ~M5UnitQRCode(void) = default;

    virtual bool begin(void) = 0;
    virtual bool available(void) = 0;
    virtual String getFirmwareVersion(void) = 0;
    virtual String getBarcode(void) = 0;
};
