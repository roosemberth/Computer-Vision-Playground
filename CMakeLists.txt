cmake_minimum_required(VERSION 3.3)
project(Computer_Vision_Playground)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/SystechDefinitions.h
    src/v4l-wrapper.cpp
    src/v4l-wrapper.hpp
    LICENSE
    Makefile
    README.md)

add_executable(Computer_Vision_Playground ${SOURCE_FILES})