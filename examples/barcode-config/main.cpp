#include <M5Unified.h>

#include "M5UnitQRCodeUART.hpp"

M5UnitQRCodeUART barcode(Serial1, UART_RXD, UART_TXD);

void setup(void) {
    M5.begin();
    barcode.begin();
    barcode.setStartTone(M5UnitQRCodeUART::StartTone::Off);
    // barcode.setReadSuccessTone(M5UnitQRCodeUART::ReadSuccessTone::Off); //
    // 効かない
    barcode.setReadSuccessToneTimes(
        M5UnitQRCodeUART::ReadSuccessToneTimes::Zero);
}

void loop(void) {
    M5.update();
    if (barcode.available()) {
        M5_LOGI("barcode: %s", barcode.getBarcode().c_str());
    }
    delay(10);
}
