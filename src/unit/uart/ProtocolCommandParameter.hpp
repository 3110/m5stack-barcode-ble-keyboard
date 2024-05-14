#pragma once

#include <Arduino.h>

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

inline const char* getName(ProtocolPackageType packageType) {
    switch (packageType) {
        case ProtocolPackageType::ConfigurationWrite:
            return "ConfigurationWrite";
        case ProtocolPackageType::ConfigurationWriteReply:
            return "ConfigurationWriteReply";
        case ProtocolPackageType::ConfigurationRead:
            return "ConfigurationRead";
        case ProtocolPackageType::ConfigurationReadReply:
            return "ConfigurationReadReply";
        default:
            return "Unknown";
    }
}

enum class StartTone : uint8_t
{
    Off = 0x00,
    FourTones = 0x01,
    TwoTones = 0x02,
};

inline const char* getName(StartTone startTone) {
    switch (startTone) {
        case StartTone::Off:
            return "Off";
        case StartTone::FourTones:
            return "FourTones";
        case StartTone::TwoTones:
            return "TwoTones";
        default:
            return "Unknown";
    }
}

enum class ReadSuccessTone : uint8_t
{
    Off = 0x00,
    On = 0x01,
};

inline const char* getName(ReadSuccessTone readSuccessTone) {
    switch (readSuccessTone) {
        case ReadSuccessTone::Off:
            return "Off";
        case ReadSuccessTone::On:
            return "On";
        default:
            return "Unknown";
    }
}

enum class ReadSuccessToneTimes : uint8_t
{
    Zero = 0x00,
    One = 0x01,
    Two = 0x02,
};

inline const char* getName(ReadSuccessToneTimes readSuccessToneTimes) {
    switch (readSuccessToneTimes) {
        case ReadSuccessToneTimes::Zero:
            return "Zero";
        case ReadSuccessToneTimes::One:
            return "One";
        case ReadSuccessToneTimes::Two:
            return "Two";
        default:
            return "Unknown";
    }
}

enum class BurstMode : uint8_t
{
    Continuous = 0x00,
    Automatic = 0x01,
    Pulse = 0x02,
    MotionInduction = 0x04,
    AutoScan = 0x05,
};

inline const char* getName(BurstMode burstMode) {
    switch (burstMode) {
        case BurstMode::Continuous:
            return "Continuous";
        case BurstMode::Automatic:
            return "Automatic";
        case BurstMode::Pulse:
            return "Pulse";
        case BurstMode::MotionInduction:
            return "MotionInduction";
        case BurstMode::AutoScan:
            return "AutoScan";
        default:
            return "Unknown";
    }
}
