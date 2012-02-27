cmake_minimum_required(VERSION 2.6)
project(server-example)

include(FindPkgConfig)

pkg_check_modules(DEPS
                  glib-2.0
                  gio-2.0
                  dpl-efl
                  dpl-dbus-efl
                  REQUIRED)

set(TARGET_NAME "server-example")

set(SRCS
    server-example.cpp)

include_directories(${DEPS_INCLUDE_DIRS})

add_definitions("-std=c++0x")
add_definitions("-pedantic")
add_definitions("-Wall")
add_definitions("-DDPL_LOGS_ENABLED")

add_executable(${TARGET_NAME} ${SRCS})
target_link_libraries(${TARGET_NAME} ${DEPS_LIBRARIES})