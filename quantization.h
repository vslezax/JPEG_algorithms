#ifndef JPEG_QUANTIZATION_H
#define JPEG_QUANTIZATION_H

#include <vector>
#include <iostream>
#include "pixels.h"

using data = std::vector<std::vector<int>>;

[[maybe_unused]] int Qluma[8][8] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 36, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
};

[[maybe_unused]] int Qchroma[8][8] = {
        {17, 18, 24, 47, 99, 99, 99, 99},
        {18, 21, 26, 66, 99, 99, 99, 99},
        {24, 26, 56, 99, 99, 99, 99, 99},
        {47, 66, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99}
};

data Q(int R, int blockSize){
    data result(blockSize, std::vector<int>(blockSize));

    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            result[i][j] = 1 + (i + j) * R;
        }
    }

    return result;
}

data forwardQuantization(const data& block, int blockSize, data q){
    data result(blockSize, std::vector<int>(blockSize));

    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            result[i][j] = (int)(block[i][j] / q[i][j]);
        }
    }

    return result;
}

data backwardQuantization(const data& block, int blockSize, data q){
    data result(blockSize, std::vector<int>(blockSize));

    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            result[i][j] = block[i][j] * q[i][j];
        }
    }

    return result;
}

data QuantizateImage(const data& Y, data(*func)(const data&, int, data), int blockSize, int R){
    int H = Y.size();
    int W = Y[0].size();
    data outputData(H, std::vector<int>(W, 0));
    data block(blockSize, std::vector<int>(blockSize, 0));

    // Tiling entire image
    auto q = Q(R, blockSize);
    for (int i = 0; i < H; i += blockSize){
        int errors = 0;
        for (int j = 0; j < W; j += blockSize){
            // Forming block
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    block.at(k).at(l) = Y.at(i + k).at(j + l);
                }
            }

            // Quantization
            data quantization = func(block, blockSize, q);

            // Forming outputData
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    outputData.at(i + k).at(j + l) = quantization.at(k).at(l);
                }
            }
        }
    }
    return outputData;
}

#endif //JPEG_QUANTIZATION_H
