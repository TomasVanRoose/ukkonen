# Version
cmake_minimum_required(VERSION 3.1...3.12)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -Wextra -Wformat=2 -Winit-self -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wconversion -Werror=format-security -Werror=implicit-function-declaration")

project(Ukkonen VERSION 1.0 LANGUAGES C)

#add_library(ukklib STATIC src/main.c)

add_executable(ukkonen src/main.c src/suffix.c)
