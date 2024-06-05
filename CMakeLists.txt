cmake_minimum_required(VERSION 3.25)

project(dbus_ble)

include(FindPkgConfig)
pkg_check_modules(dbus-cxx REQUIRED dbus-cxx-2.0)

add_executable(dbus-ble ${CMAKE_CURRENT_SOURCE_DIR}/src/ble_test.cpp src/parse_dbus.cpp src/local_adapter.cpp src/found_ble.cpp src/encode.cpp src/websocket.cpp src/threadpool.cpp)

target_include_directories(dbus-ble PRIVATE ${dbus-cxx_INCLUDE_DIRS} include)
target_link_libraries(dbus-ble ${dbus-cxx_LDFLAGS} -lcrypto)
