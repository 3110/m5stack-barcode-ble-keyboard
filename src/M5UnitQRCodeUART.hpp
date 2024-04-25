#pragma once

#include "M5UnitQRCode.hpp"

class M5UnitQRCodeUART : public M5UnitQRCode {
public:
    static constexpr uint32_t DEFAULT_BAUDRATE = 115200;
    static constexpr size_t RX_BUFSIZ = UINT8_MAX + 1;

    static constexpr uint32_t COMMAND_INTERVAL_MS = 100;

    static constexpr size_t COMMAND_TYPE_OFFSET = 0;
    static constexpr size_t PID_OFFSET = COMMAND_TYPE_OFFSET + 1;
    static constexpr size_t FID_OFFSET = PID_OFFSET + 1;
    static constexpr size_t PARAMETER_SIZE_OFFSET = FID_OFFSET + 1;
    static constexpr size_t PARAMETER_VALUE_OFFSET = PARAMETER_SIZE_OFFSET + 2;

    enum class ProtocolPackageType : uint8_t
    {
        ConfigurationWrite = 0x21,
        ConfigurationWriteReply = 0x22,
        ConfigurationRead = 0x23,
        ConfigurationReadReply = 0x24,
        ControlInstruction = 0x32,
        ControlReply = 0x33,
        StatusRead = 0x43,
        StatusReply = 0x44,
        ImageRead = 0x60,
        ImageReply = 0x61,
    };

    enum class StartTone : uint8_t
    {
        Off = 0x00,
        FourTones = 0x01,
        TwoTones = 0x02,
    };

    enum class ReadSuccessTone : uint8_t
    {
        Off = 0x00,
        On = 0x01,
    };

    enum class ReadSuccessToneTimes : uint8_t
    {
        Zero = 0x00,
        One = 0x01,
        Two = 0x02,
    };

    M5UnitQRCodeUART(HardwareSerial& serial, uint8_t rx, uint8_t tx,
                     uint32_t baudrate = DEFAULT_BAUDRATE);
    virtual ~M5UnitQRCodeUART(void) = default;

    virtual bool begin(void) override;
    virtual bool available(void) override;

    virtual String getFirmwareVersion(void) override;
    virtual String getBarcode(void) override;

    virtual bool setStartTone(StartTone tone);
    virtual bool setReadSuccessTone(ReadSuccessTone tone);
    virtual bool setReadSuccessToneTimes(ReadSuccessToneTimes times);

protected:
    virtual bool send(const uint8_t* data, size_t size);
    virtual bool receive(void);
    virtual bool flush(void);
    virtual void clearRXBuffer(void);

    virtual uint8_t getReceivedType(void) const;
    virtual uint8_t getReceivedPID(void) const;
    virtual uint8_t getReceivedFID(void) const;
    virtual size_t getReceivedParameterSize(void) const;
    virtual String getReceivedParameter(void) const;

    virtual bool isValidReply(ProtocolPackageType type) const;
    virtual bool isValidID(uint8_t pid, uint8_t fid) const;

    virtual bool sendConfigurationWrite(uint8_t pid, uint8_t fid,
                                        const uint8_t* param, uint16_t size);
    virtual bool sendStatusQuery(uint8_t pid, uint8_t fid);

private:
    HardwareSerial* _serial;
    uint8_t _rx_pin;
    uint8_t _tx_pin;
    uint32_t _baudrate;
    char _rx_buf[RX_BUFSIZ];
    uint8_t _rx_buf_pos;
};
