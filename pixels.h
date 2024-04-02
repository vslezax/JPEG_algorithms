#ifndef JPEG_PIXELS_H
#define JPEG_PIXELS_H

#include <vector>
#include <iostream>
#include <climits>
#include "Windows.h"
#include "cmath"

unsigned char clipping(double value){
    if (value < 0) return 0;
    if (value > UCHAR_MAX) return UCHAR_MAX;
    return (char)value;
}

struct RGBPixel {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    RGBPixel(RGBPixel *pPixel) {
        R = pPixel->R;
        G = pPixel->G;
        B = pPixel->B;
    }
    RGBPixel(int red, int blue, int green) {
        this->R = red;
        this->G = green;
        this->B = blue;
    }
};

struct YCbCrPixel {
    unsigned char Y;
    unsigned char Cb;
    unsigned char Cr;
    YCbCrPixel() {
        Y = 0;
        Cb = 0;
        Cr = 0;
    }
    YCbCrPixel(unsigned char a, unsigned char b, unsigned char c){
        Y = a;
        Cb = b;
        Cr = c;
    }
};


#endif //JPEG_PIXELS_H
