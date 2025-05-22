cmake_minimum_required(VERSION 3.5)

if(IDF_VERSION_MAJOR GREATER_EQUAL 4)
    idf_component_register(
        SRC_DIRS src
        INCLUDE_DIRS src
        REQUIRES log driver)
else()
    set(COMPONENT_SRCDIRS src)
    set(COMPONENT_ADD_INCLUDEDIRS src)
    set(COMPONENT_REQUIRES log driver)
    register_component()
endif()
