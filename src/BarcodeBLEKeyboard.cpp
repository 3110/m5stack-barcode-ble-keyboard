#include "BarcodeBLEKeyboard.hpp"

BarcodeBLEKeyboard::BarcodeBLEKeyboard(M5UnitQRCode* barcode)
    : _barcode(barcode), _keyboard("M5Barcode") {
}

BarcodeBLEKeyboard::~BarcodeBLEKeyboard(void) {
    if (this->_barcode) {
        delete this->_barcode;
        this->_barcode = nullptr;
    }
}

bool BarcodeBLEKeyboard::begin(std::function<bool(void)> onConnected,
                               std::function<bool(void)> onDisconnected) {
    if (!this->_keyboard.begin(onConnected, onDisconnected)) {
        return false;
    }
    if (!this->_barcode->begin()) {
        return false;
    }
    return true;
}

bool BarcodeBLEKeyboard::update(void) {
    return this->_keyboard.update();
}

bool BarcodeBLEKeyboard::available(void) {
    return this->_barcode->available();
}

String BarcodeBLEKeyboard::getFirmwareVersion(void) {
    return this->_barcode->getFirmwareVersion();
}

String BarcodeBLEKeyboard::getBarcode(void) {
    return this->_barcode->getBarcode();
}

bool BarcodeBLEKeyboard::send(const String& s) {
    return this->_keyboard.write(s.c_str(), s.length());
}

bool BarcodeBLEKeyboard::send(uint8_t c) {
    return this->_keyboard.write(c);
}
