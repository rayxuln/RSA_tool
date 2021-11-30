cmake_minimum_required(VERSION 3.19)
project(RSA_tool)

if(CMAKE_COMPILER_IS_GNUCXX)
    message("Using GNU compiler...")
    set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 20)

add_executable(RSA_tool main.cpp RSA.cpp RSA.h Integer.h)