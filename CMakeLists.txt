
cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(airbrakes_sdk
    LANGUAGES C CXX
    VERSION "0.1.0"
)

add_library(airbrakes_sdk OBJECT
    src/i2c.cc
    src/mutex.cc
)

target_include_directories(airbrakes_sdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# link to stm32cubemx interface target to get parent project headers
target_link_libraries(airbrakes_sdk PUBLIC stm32cubemx)
