#pragma once

#include "M5UnitQRCode.hpp"

class M5UnitQRCodeI2C : public M5UnitQRCode {
public:
    static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x21;
    static constexpr uint32_t DEFAULT_I2C_FREQUENCY = 100000L;

    enum class TriggerMode : uint8_t
    {
        Auto = 0,
        Manual = 1,
    };

    enum class Register : uint16_t
    {
        Trigger = 0x0000,
        DataStatus = 0x0010,
        DataLength = 0x0020,
        TriggerMode = 0x0030,
        TriggerButton = 0x0040,
        Data = 0x1000,
        FirmwareVersion = 0x00FE,
        I2CAddress = 0x00FF,
    };

    enum class DataStatus : uint8_t
    {
        NotReady = 0,
        Ready = 1,
        ReadAgain = 2,
    };

    enum class TriggerButtonStatus : uint8_t
    {
        Pressed = 0,
        NotPressed = 1,
    };

    M5UnitQRCodeI2C(TwoWire& wire, uint8_t scl_pin, uint8_t sda_pin,
                    uint8_t addr = DEFAULT_I2C_ADDRESS,
                    uint32_t frequency = DEFAULT_I2C_FREQUENCY);
    virtual ~M5UnitQRCodeI2C(void) = default;

    virtual bool begin(void) override;
    virtual bool available(void) override;

    virtual String getFirmwareVersion(void) override;
    virtual String getBarcode(void) override;

    virtual TriggerMode getTriggerMode(void);
    virtual void setTriggerMode(TriggerMode mode);
    virtual bool triggered(void);

protected:
    virtual DataStatus getStatus(void);
    virtual uint16_t getDataLength(void);
    virtual void writeBytes(Register reg, const uint8_t* data, uint8_t len);
    virtual void readBytes(Register reg, uint8_t* data, uint8_t len);

private:
    TwoWire* _wire;
    uint8_t _addr;
    uint8_t _scl_pin;
    uint8_t _sda_pin;
    uint32_t _frequency;
};
