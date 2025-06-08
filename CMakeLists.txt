idf_component_register(
    SRCS
        "src/ws2812_control.c"     # C source
    INCLUDE_DIRS
        "include"                  # makes <ws2812_cpp.hpp> visible everywhere
    REQUIRES
        driver                     # uses RMT, GPIO, etc.
        freertos                   # if your C++ needs vTaskDelay, xQueue*
)

# (optional) clip the C++ standard or extra flags
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++23)
