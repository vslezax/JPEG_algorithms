cmake_minimum_required(VERSION 3.27)
project(JPEG)

set(CMAKE_CXX_STANDARD 17)

add_executable(JPEG main.cpp
        pixels.h
        YCbCr.h
        DCT.h
        Image.h
        PSNR.h
        quantization.h
        Utils.h
)
