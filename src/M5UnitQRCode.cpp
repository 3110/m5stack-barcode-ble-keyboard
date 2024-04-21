#include "M5UnitQRCode.hpp"

M5UnitQRCode::M5UnitQRCode(void) {
#if defined(LOG_LOCAL_LEVEL)
    esp_log_level_set(STR(ESP_LOG_TAG), LOG_LOCAL_LEVEL);
#endif
}
