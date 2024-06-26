#include <M5Unified.h>

#include "unit/uart/M5UnitQRCodeUART.hpp"

M5UnitQRCodeUART barcode(Serial1, UNIT_UART_RXD, UNIT_UART_TXD);

void setup(void) {
    M5.begin();
    barcode.begin();
    barcode.setStartTone(StartTone::Off);
    // barcode.setReadSuccessTone(M5UnitQRCodeUART::ReadSuccessTone::Off); //
    // 効かない
    barcode.setReadSuccessToneTimes(ReadSuccessToneTimes::Zero);
    BurstMode burstMode = BurstMode::MotionInduction;
    if (barcode.setBurstMode(burstMode)) {
        M5_LOGI("Change Burst Mode: %s", getName(burstMode));
    }
    if (barcode.readBurstMode(burstMode)) {
        M5_LOGI("Burst Mode: %s", getName(burstMode));
    }
}

void loop(void) {
    M5.update();
    if (barcode.available()) {
        M5_LOGI("barcode: %s", barcode.getBarcode().c_str());
    }
    delay(10);
}
