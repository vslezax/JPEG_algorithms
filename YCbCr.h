#ifndef JPEG_YCBCR_H
#define JPEG_YCBCR_H

#include <vector>

#include "Image.h"
#include "pixels.h"

std::vector<YCbCrPixel> RGBtoYCbCr(const std::vector<RGBPixel>& pixels, int H, int W){
    std::vector<YCbCrPixel> result(H * W, YCbCrPixel());
    for (int i = 0; i < pixels.size(); i++){
        unsigned char Y = clipping(0.299*pixels[i].R + 0.587*pixels[i].G + 0.114*pixels[i].B);
        unsigned char Cb = clipping(0.5643*(pixels[i].B - Y) + 128);
        unsigned char Cr = clipping(0.7132*(pixels[i].R - Y) + 128);
        result[i] = YCbCrPixel(Y, Cb, Cr);
    }

    return result;
}

std::vector<std::vector<YCbCrPixel>> RGBtoYCbCr(const std::vector<std::vector<RGBPixel>>& pixels, int H, int W){
    std::vector<std::vector<YCbCrPixel>> result(H, std::vector<YCbCrPixel>(W, {0, 0, 0}));
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            unsigned char Y = clipping(0.299*pixels[i][j].R + 0.587*pixels[i][j].G + 0.114*pixels[i][j].B);
            unsigned char Cb = clipping(0.5643*(pixels[i][j].B - Y) + 128);
            unsigned char Cr = clipping(0.7132*(pixels[i][j].R - Y) + 128);
            result[i][j] = YCbCrPixel(Y, Cb, Cr);
        }
    }

    return result;
}

std::vector<RGBPixel> YCbCrtoRGB(std::vector<YCbCrPixel>& Y, int H, int W){
    std::vector<RGBPixel> newPixels(H * W, RGBPixel(0, 0, 0));
    for (int i = 0; i < H * W; i++){
        unsigned char R = clipping(Y[i].Y - 0.714*(Y[i].Cr - 128) - 0.334*(Y[i].Cb - 128));
        unsigned char G = clipping(Y[i].Y + 1.402*(Y[i].Cr - 128));
        unsigned char B = clipping(Y[i].Y + 1.772*(Y[i].Cb - 128));
        newPixels[i] = RGBPixel(R, G, B);
    }

    return newPixels;
}

std::vector<std::vector<unsigned char>> returnY(const std::string& inputPath, const std::string& yPath){
    // Read data
    Image src(inputPath);
    Image yImage(yPath);
    src.readData();
    int H = src.H;
    int W = src.W;
    std::vector<std::vector<YCbCrPixel>> srcDataFull = RGBtoYCbCr(src.data, H, W);
    std::vector<std::vector<unsigned char>> srcData(H, std::vector<unsigned char>(W, 0));

    // Data -> Y
    std::vector<std::vector<unsigned char>> Y(H, std::vector<unsigned char>(W, 0));
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            srcData.at(i).at(j) = srcDataFull.at(i).at(j).Y;
            Y.at(i).at(j) = srcData.at(i).at(j);
        }
    }
    yImage.writeChannel(src.fileHeader, src.infoHeader, Y);

    return Y;
}

#endif //JPEG_YCBCR_H
