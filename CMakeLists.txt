idf_component_register(
    SRCS
        "src/ws2812_control.c"
        "src/rmt_wrapper.cpp"
        "src/ws2812_cpp.cpp"
        "src/ws2812_effects.cpp"
        "src/ws2812_multi_animator.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
        driver
        freertos
)

# (optional) clip the C++ standard or extra flags
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++23)
