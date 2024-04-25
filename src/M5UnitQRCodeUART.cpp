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

String M5UnitQRCodeUART::getFirmwareVersion(void) {
    if (sendStatusQuery(0x02, 0xC1)) {
        return getReceivedParameter();
    } else {
        return "";
    }
}

String M5UnitQRCodeUART::getBarcode(void) {
    return String(this->_rx_buf);
}

bool M5UnitQRCodeUART::readBurstMode(BurstMode& burstMode) {
    uint8_t param;
    if (sendConfigurationRead(0x61, 0x41, param)) {
        burstMode = static_cast<BurstMode>(param);
        return true;
    } else {
        return false;
    }
}

bool M5UnitQRCodeUART::setBurstMode(BurstMode burstMode) {
    const uint8_t params[] = {static_cast<uint8_t>(burstMode)};
    return sendConfigurationWrite(0x61, 0x41, params,
                                  sizeof(params) / sizeof(params[0]));
}

bool M5UnitQRCodeUART::setStartTone(StartTone tone) {
    const uint8_t params[] = {static_cast<uint8_t>(tone)};
    return sendConfigurationWrite(0x63, 0x45, params,
                                  sizeof(params) / sizeof(params[0]));
}

bool M5UnitQRCodeUART::setReadSuccessTone(ReadSuccessTone tone) {
    const uint8_t params[] = {static_cast<uint8_t>(tone)};
    return sendConfigurationWrite(0x63, 0x46, params,
                                  sizeof(params) / sizeof(params[0]));
}

bool M5UnitQRCodeUART::setReadSuccessToneTimes(ReadSuccessToneTimes times) {
    const uint8_t params[] = {static_cast<uint8_t>(times)};
    return sendConfigurationWrite(0x63, 0x42, params,
                                  sizeof(params) / sizeof(params[0]));
}

bool M5UnitQRCodeUART::send(const uint8_t* data, size_t size) {
    ESP_LOGD(STR(ESP_LOG_TAG), "Sending:");
    ESP_LOG_BUFFER_HEXDUMP(STR(ESP_LOG_TAG), data, size, ESP_LOG_DEBUG);
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
    ESP_LOGD(STR(ESP_LOG_TAG), "Receiving:");
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

bool M5UnitQRCodeUART::sendConfigurationWrite(uint8_t pid, uint8_t fid,
                                              const uint8_t* param,
                                              uint16_t size) {
    const uint8_t len = (fid >> 6);
    uint8_t cmd[3 + size + (len == 3 ? 2 : 0)] = {
        static_cast<uint8_t>(ProtocolPackageType::ConfigurationWrite),
        pid,
        fid,
    };
    if (len == 3) {
        cmd[3] = highByte(size);
        cmd[4] = lowByte(size);
        for (size_t i = 0; i < size; ++i) {
            cmd[5 + i] = param[i];
        }
    } else if (len == 2) {
        cmd[3] = param[0];
        cmd[4] = param[1];
    } else if (len == 1) {
        cmd[3] = param[0];
    }
    ESP_LOG_BUFFER_HEXDUMP(STR(ESP_LOG_TAG), cmd, sizeof(cmd) / sizeof(cmd[0]),
                           ESP_LOG_DEBUG);
    if (!send(cmd, sizeof(cmd) / sizeof(cmd[0]))) {
        ESP_LOGE(STR(ESP_LOG_TAG),
                 "Failed to send command: 0x%02X 0x%02X 0x%02X", cmd[0], cmd[1],
                 cmd[2]);
        return false;
    }
    delay(COMMAND_INTERVAL_MS);
    if (!receive()) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Failed to receive data.");
        return false;
    }
    if (!isValidReply(ProtocolPackageType::ConfigurationWriteReply)) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Illegal Reply Type: 0x%02X",
                 this->_rx_buf[COMMAND_TYPE_OFFSET]);
        return false;
    }
    if (!isValidID(pid, fid)) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Illegal reply: 0x%02X 0x%02X 0x%02X",
                 getReceivedType(), getReceivedPID(), getReceivedFID());
    }
    if (len == 1) {
        if (this->_rx_buf[3] != param[0]) {
            ESP_LOGE(STR(ESP_LOG_TAG),
                     "PAR is not equal to the sent value: expected = 0x%02X, "
                     "actual = 0x%02X",
                     param[0], this->_rx_buf[3]);
            return false;
        }
    } else {
        if (this->_rx_buf[3] != 0x00) {
            ESP_LOGE(STR(ESP_LOG_TAG),
                     "PAR is invalid: expected = 0x00, actual = 0x%02X",
                     this->_rx_buf[3]);
            return false;
        }
    }
    if (this->_rx_buf[4] == 0x01) {
        ESP_LOGE(STR(ESP_LOG_TAG),
                 "Invalid PID/FID: pid = 0x%02X, fid = 0x%02X", pid, fid);
        return false;
    }
    return true;
}

bool M5UnitQRCodeUART::sendConfigurationRead(uint8_t pid, uint8_t fid,
                                             uint8_t& param) {
    const uint8_t t =
        static_cast<uint8_t>(ProtocolPackageType::ConfigurationRead);
    const uint8_t cmd[] = {t, pid, fid};
    if (!send(cmd, sizeof(cmd) / sizeof(cmd[0]))) {
        ESP_LOGE(STR(ESP_LOG_TAG),
                 "Failed to send command: 0x%02X 0x%02X 0x%02X", t, pid, fid);
        return false;
    }
    delay(COMMAND_INTERVAL_MS);
    if (!receive()) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Failed to receive data.");
        return false;
    }
    if (!isValidReply(ProtocolPackageType::ConfigurationReadReply)) {
        ESP_LOGE(STR(ESP_LOG_TAG), "Illegal Reply Type: 0x%02X",
                 this->_rx_buf[COMMAND_TYPE_OFFSET]);
        return false;
    }
    param = this->_rx_buf[PARAMETER_SIZE_OFFSET];
    return true;
}

bool M5UnitQRCodeUART::sendStatusQuery(uint8_t pid, uint8_t fid) {
    const uint8_t t = static_cast<uint8_t>(ProtocolPackageType::StatusRead);
    const uint8_t cmd[] = {t, pid, fid};
    if (!send(cmd, sizeof(cmd) / sizeof(cmd[0]))) {
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
