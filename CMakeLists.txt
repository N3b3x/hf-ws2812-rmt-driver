idf_component_register(
    SRCS
        "src/ws2812_control.c"
        "src/ws2812_cpp.cpp"
        "src/ws2812_effects.cpp"
        "src/ws2812_multi_animator.cpp"
        "src/led_strip_encoder.c"
    INCLUDE_DIRS
        "include"
    REQUIRES
        driver
        freertos
)

# (optional) clip the C++ standard or extra flags
# Use C++17 for compatibility with ESP-IDF toolchains
target_compile_options(${COMPONENT_LIB} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>)
