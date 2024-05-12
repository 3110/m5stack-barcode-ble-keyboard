#include <M5Unified.h>

#include "BarcodeBLEKeyboard.hpp"

#if defined(USE_I2C)
#include "i2c/M5UnitQRCodeI2C.hpp"
#elif defined(USE_UART)
#include "uart/M5UnitQRCodeUART.hpp"
#else
#error "Please define the protocol USE_I2C or USE_UART"
#endif

inline M5UnitQRCode* getInstance(void) {
#if defined(USE_I2C)
    return new M5UnitQRCodeI2C(Wire, I2C_SCL, I2C_SDA);
#elif defined(USE_UART)
    return new M5UnitQRCodeUART(Serial1, UART_RXD, UART_TXD);
#else
#error "Please define the protocol USE_I2C or USE_UART"
#endif
    return nullptr;
}

bool newlineMode = false;
BarcodeBLEKeyboard keyboard(getInstance());

#if defined(HAS_LED)
#define FASTLED_INTERNAL
#include <FastLED.h>

CRGB led[NUM_LEDS] = {
    CRGB::Black,
};

inline void showLED(const CRGB& color, size_t n_leds = NUM_LEDS) {
    for (size_t i = 0; i < n_leds; ++i) {
        led[i] = color;
    }
    FastLED.show();
}
#endif

bool onConnect(void) {
#if defined(HAS_LED)
    showLED(CRGB::Green);
#endif
    return true;
}

bool onDisconnect(void) {
#if defined(HAS_LED)
    showLED(CRGB::Red);
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
    if (M5.BtnA.wasPressed()) {
        newlineMode = !newlineMode;
        showLED(newlineMode ? CRGB::Blue : CRGB::Green);
    }
    keyboard.update();
    if (keyboard.available()) {
        const String code = keyboard.getBarcode();
        M5_LOGI("barcode: %s", code.c_str());
        keyboard.send(code);
        if (newlineMode) {
            delay(NEWLINE_MODE_DELAY_MS);
            keyboard.send(KEY_RETURN);
        }
    }
    delay(10);
}
