#include <M5Unified.h>

#include "BarcodeBLEKeyboard.hpp"

#if defined(USE_UNIT_I2C)
#include "unit/i2c/M5UnitQRCodeI2C.hpp"
#elif defined(USE_UNIT_UART)
#include "unit/uart/M5UnitQRCodeUART.hpp"
#else
#error "Please define the protocol USE_UNIT_I2C or USE_UNIT_UART"
#endif

inline M5UnitQRCode* getInstance(void) {
#if defined(USE_UNIT_I2C)
    return new M5UnitQRCodeI2C(Wire, UNIT_I2C_SCL, UNIT_I2C_SDA);
#elif defined(USE_UNIT_UART)
    return new M5UnitQRCodeUART(Serial1, UNIT_UART_RXD, UNIT_UART_TXD);
#else
#error "Please define the protocol USE_UNIT_I2C or USE_UNIT_UART"
#endif
    return nullptr;
}

#if defined(ENABLE_NEWLINE_MODE)
bool newlineMode = false;
#endif

BarcodeBLEKeyboard keyboard(getInstance());

#if defined(HAS_LED)
#define FASTLED_INTERNAL
#include <FastLED.h>

#if defined(HAS_LED)
const CRGB LED_CONNECTED = CRGB::Green;
const CRGB LED_DISCONNECTED = CRGB::Black;
#if defined(ENABLE_NEWLINE_MODE)
const CRGB LED_NEWLINE_MODE = CRGB::Yellow;
#endif
const CRGB LED_ISBN13_VALID = CRGB::Blue;
const CRGB LED_ISBN13_INVALID = CRGB::Red;
CRGB led[NUM_LEDS] = {
    LED_DISCONNECTED,
};
#endif

CRGB connectedLED = LED_CONNECTED;

inline void showLED(const CRGB& color, size_t n_leds = NUM_LEDS) {
    for (size_t i = 0; i < n_leds; ++i) {
        led[i] = color;
    }
    FastLED.show();
}
#endif

bool onConnect(void) {
#if defined(HAS_LED)
    showLED(connectedLED);
#endif
    return true;
}

bool onDisconnect(void) {
#if defined(HAS_LED)
    showLED(LED_DISCONNECTED);
#endif
    return true;
}

void setup(void) {
    M5.begin();
#if defined(HAS_LED)
    FastLED.addLeds<WS2812, RGB_LED_PIN, RGB_ORDER>(led, NUM_LEDS);
    FastLED.setBrightness(20);
    FastLED.show();
#endif
    keyboard.begin(onConnect, onDisconnect);
}

void loop(void) {
    M5.update();
    keyboard.update();
    if (keyboard.available()) {
        const String code = keyboard.getBarcode();
        M5_LOGI("barcode: %s", code.c_str());
        keyboard.send(code);
    }
    delay(10);
}
