cmake_minimum_required(VERSION 2.6)
project(client-example)

include(FindPkgConfig)

pkg_check_modules(DEPS
                  dbus-1
                  dpl-efl
                  dpl-dbus-efl
                  REQUIRED)

set(TARGET_NAME "client-example")

set(SRCS
    client-example.cpp)

include_directories(${DEPS_INCLUDE_DIRS})

add_definitions("-std=c++0x")
add_definitions("-DDPL_LOGS_ENABLED")

add_executable(${TARGET_NAME} ${SRCS})
target_link_libraries(${TARGET_NAME} ${DEPS_LIBRARIES})

