#include "M5UnitQRCodeUART.hpp"

M5UnitQRCodeUART::M5UnitQRCodeUART(HardwareSerial& serial, uint8_t rx,
                                   uint8_t tx, uint32_t baudrate)
    : _serial(&serial),
      _rx_pin(rx),
      _tx_pin(tx),
      _baudrate(baudrate),
      _rx_buf{0},
      _rx_buf_pos(0) {
}

bool M5UnitQRCodeUART::begin(void) {
    this->_serial->begin(this->_baudrate, SERIAL_8N1, this->_rx_pin,
                         this->_tx_pin);
#if defined(LOG_LOCAL_LEVEL)
    esp_log_level_set(STR(ESP_LOG_TAG), LOG_LOCAL_LEVEL);
#endif
    ESP_LOGI(STR(ESP_LOG_TAG), "Firmware Version %s",
             getFirmwareVersion().c_str());
    return true;
}

bool M5UnitQRCodeUART::available(void) {
    return receive();
}

String M5UnitQRCodeUART::getBarcode(void) {
    return String(this->_rx_buf);
}

String M5UnitQRCodeUART::getFirmwareVersion(void) {
    if (sendStatusQuery(0x02, 0xC1)) {
        return getReceivedParameter();
    } else {
        return "";
    }
}

bool M5UnitQRCodeUART::setStartTone(StartTone tone) {
    const uint8_t cmd[] = {
        static_cast<uint8_t>(ProtocolPackageType::ConfigurationWrite), 0x63,
        0x45, static_cast<uint8_t>(tone)};
    send(cmd, sizeof(cmd));
    delay(COMMAND_INTERVAL_MS);
    flush();  // XXX
    return true;
}

bool M5UnitQRCodeUART::setReadSuccessTone(ReadSuccessTone tone) {
    const uint8_t cmd[] = {
        static_cast<uint8_t>(ProtocolPackageType::ConfigurationWrite), 0x63,
        0x46, static_cast<uint8_t>(tone)};
    send(cmd, sizeof(cmd));
    delay(COMMAND_INTERVAL_MS);
    flush();  // XXX
    return true;
}

bool M5UnitQRCodeUART::setReadSuccessToneTimes(ReadSuccessToneTimes times) {
    const uint8_t cmd[] = {
        static_cast<uint8_t>(ProtocolPackageType::ConfigurationWrite), 0x63,
        0x42, static_cast<uint8_t>(times)};
    send(cmd, sizeof(cmd));
    delay(COMMAND_INTERVAL_MS);
    flush();  // XXX
    return true;
}

bool M5UnitQRCodeUART::send(const uint8_t* data, size_t size) {
    return this->_serial->write(data, size) == size;
}

bool M5UnitQRCodeUART::receive(void) {
    clearRXBuffer();
    if (!this->_serial->available()) {
        ESP_LOGV(STR(ESP_LOG_TAG), "No data available.");
        return false;
    }
    while (this->_serial->available()) {
        this->_rx_buf[this->_rx_buf_pos++] = this->_serial->read();
    }
    ESP_LOG_BUFFER_HEXDUMP(STR(ESP_LOG_TAG), this->_rx_buf, this->_rx_buf_pos,
                           ESP_LOG_DEBUG);
    return true;
}

bool M5UnitQRCodeUART::flush(void) {
    bool flushed = this->_serial->available();
    while (this->_serial->available()) {
        this->_serial->read();
    }
    return flushed;
}

void M5UnitQRCodeUART::clearRXBuffer(void) {
    memset(this->_rx_buf, 0, sizeof(this->_rx_buf));
    this->_rx_buf_pos = 0;
}

uint8_t M5UnitQRCodeUART::getReceivedType(void) const {
    return this->_rx_buf[COMMAND_TYPE_OFFSET];
}

uint8_t M5UnitQRCodeUART::getReceivedPID(void) const {
    return this->_rx_buf[PID_OFFSET];
}

uint8_t M5UnitQRCodeUART::getReceivedFID(void) const {
    return this->_rx_buf[FID_OFFSET];
}

size_t M5UnitQRCodeUART::getReceivedParameterSize(void) const {
    return (this->_rx_buf[PARAMETER_SIZE_OFFSET] << 8) +
           this->_rx_buf[PARAMETER_SIZE_OFFSET + 1];
}

String M5UnitQRCodeUART::getReceivedParameter(void) const {
    return String(&this->_rx_buf[PARAMETER_VALUE_OFFSET]);
}

bool M5UnitQRCodeUART::isValidReply(ProtocolPackageType type) const {
    return getReceivedType() == static_cast<uint8_t>(type);
}

bool M5UnitQRCodeUART::isValidID(uint8_t pid, uint8_t fid) const {
    return getReceivedPID() == pid && getReceivedFID() == fid;
}

bool M5UnitQRCodeUART::sendStatusQuery(uint8_t pid, uint8_t fid) {
    const uint8_t t = static_cast<uint8_t>(ProtocolPackageType::StatusRead);
    const uint8_t cmd[] = {t, pid, fid};
    if (!send(cmd, sizeof(cmd))) {
        ESP_LOGE(STR(ESP_LOG_TAG),
                 "Failed to send command: 0x%02X 0x%02X 0x%02X", t, pid, fid);
        return false;
    }
    delay(COMMAND_INTERVAL_MS);
    if (!receive()) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Failed to receive data.");
        return false;
    }
    if (!isValidReply(ProtocolPackageType::StatusReply)) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Illegal Reply Type: 0x%02X",
                 this->_rx_buf[COMMAND_TYPE_OFFSET]);
        return false;
    }
    if (this->_rx_buf_pos == 2 && this->_rx_buf[1] == 0x00) {
        ESP_LOGD(STR(ESP_LOG_TAG), "All queries is not supported");
        return false;
    }
    if (!isValidID(pid, fid)) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Illegal reply: 0x%02X 0x%02X 0x%02X",
                 getReceivedType(), getReceivedPID(), getReceivedFID());
    }
    ESP_LOGD(STR(ESP_LOG_TAG), "Received Parameter Length: %d",
             getReceivedParameterSize());
    return true;
}
