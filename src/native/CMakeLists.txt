cmake_minimum_required(VERSION 3.16)
project(nativebridge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(nativebridge SHARED
    nativebridge.h
    nativebridge.cpp
    qtwrapper.h
    qtwrapper.cpp
)

target_include_directories(nativebridge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(nativebridge PRIVATE Qt6::Core)

if(ANDROID)
    # On Android, make sure we package the Kotlin/Java classes into the APK
    set_target_properties(nativebridge PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../android
    )
endif()

# Expose the library for linking with other targets if needed
install(TARGETS nativebridge)
