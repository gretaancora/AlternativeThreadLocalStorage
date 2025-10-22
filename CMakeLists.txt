cmake_minimum_required(VERSION 4.0)
project(TLS C)

set(CMAKE_C_STANDARD 11)

add_executable(TLS main.c
        TLS.h)
