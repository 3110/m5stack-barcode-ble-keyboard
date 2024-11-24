#pragma once

#include <M5BLEKeyboard.hpp>
#include <functional>

#include "M5UnitQRCode.hpp"

class BarcodeBLEKeyboard {
public:
    BarcodeBLEKeyboard(M5UnitQRCode* barcode);
    virtual ~BarcodeBLEKeyboard(void);

    virtual bool begin(std::function<bool(void)> onConnected = nullptr,
                       std::function<bool(void)> onDisconnected = nullptr);
    virtual bool update(void);
    virtual bool isConnected(void);

    virtual bool available(void);
    virtual bool startScan(void);
    virtual bool stopScan(void);
    virtual String getFirmwareVersion(void);
    virtual String getBarcode(void);

    virtual bool send(const String& s);
    virtual bool send(uint8_t c);

private:
    M5UnitQRCode* _barcode;
    M5BLEKeyboard _keyboard;
};
