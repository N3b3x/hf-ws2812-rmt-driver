#include "ws2812_cpp.hpp"

extern "C" void app_main(void) {
  WS2812Strip strip(GPIO_NUM_1, 0, 1, WS2812Strip::LedType::RGB);
  (void)strip;
}
