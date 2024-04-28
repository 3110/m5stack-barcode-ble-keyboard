#include "i2c/M5UnitQRCodeI2C.hpp"

M5UnitQRCodeI2C::M5UnitQRCodeI2C(TwoWire& wire, uint8_t scl_pin,
                                 uint8_t sda_pin, uint8_t addr,
                                 uint32_t frequency)
    : M5UnitQRCode(),
      _wire(&wire),
      _addr(addr),
      _scl_pin(scl_pin),
      _sda_pin(sda_pin),
      _frequency(frequency) {
}

bool M5UnitQRCodeI2C::begin(void) {
    this->_wire->begin(this->_sda_pin, this->_scl_pin, this->_frequency);
    delay(10);
    ESP_LOGI(STR(ESP_LOG_TAG), "Firmware Version %s",
             getFirmwareVersion().c_str());
    return true;
}

bool M5UnitQRCodeI2C::available(void) {
    return getStatus() == DataStatus::Ready;
}

String M5UnitQRCodeI2C::getFirmwareVersion(void) {
    uint8_t v = 0;
    readBytes(Register::FirmwareVersion, &v, sizeof(v));
    return String(v);
}

String M5UnitQRCodeI2C::getBarcode(void) {
    const uint16_t len = getDataLength();
    char data[len + 1] = {0};
    readBytes(Register::Data, reinterpret_cast<uint8_t*>(data), len);
    ESP_LOG_BUFFER_HEXDUMP(STR(ESP_LOG_TAG), data, len, ESP_LOG_DEBUG);
    return String(data);
}

M5UnitQRCodeI2C::TriggerMode M5UnitQRCodeI2C::getTriggerMode(void) {
    uint8_t v = 0;
    readBytes(Register::TriggerMode, &v, sizeof(v));
    return static_cast<TriggerMode>(v);
}

void M5UnitQRCodeI2C::setTriggerMode(TriggerMode mode) {
    uint8_t m = static_cast<uint8_t>(mode);
    writeBytes(Register::TriggerMode, &m, sizeof(m));
}

bool M5UnitQRCodeI2C::triggered(void) {
    uint8_t v = 0;
    readBytes(Register::Trigger, &v, sizeof(v));
    return v == static_cast<uint8_t>(TriggerButtonStatus::Pressed);
}

M5UnitQRCodeI2C::DataStatus M5UnitQRCodeI2C::getStatus(void) {
    uint8_t v = 0;
    readBytes(Register::DataStatus, &v, sizeof(v));
    return static_cast<DataStatus>(v);
}

uint16_t M5UnitQRCodeI2C::getDataLength(void) {
    uint16_t v = 0;
    readBytes(Register::DataLength, reinterpret_cast<uint8_t*>(&v), sizeof(v));
    return v;
}

void M5UnitQRCodeI2C::writeBytes(Register reg, const uint8_t* data,
                                 uint8_t len) {
    uint16_t r = static_cast<uint16_t>(reg);
    this->_wire->beginTransmission(this->_addr);
    this->_wire->write(lowByte(r));
    this->_wire->write(highByte(r));
    this->_wire->write(data, len);
    this->_wire->endTransmission();
}

void M5UnitQRCodeI2C::readBytes(Register reg, uint8_t* data, uint8_t len) {
    uint16_t r = static_cast<uint16_t>(reg);
    this->_wire->beginTransmission(this->_addr);
    this->_wire->write(lowByte(r));
    this->_wire->write(highByte(r));
    this->_wire->endTransmission();
    this->_wire->requestFrom(this->_addr, len);
    this->_wire->readBytes(data, len);
}
