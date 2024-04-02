#ifndef JPEG_QUANTIZATION_H
#define JPEG_QUANTIZATION_H

#include <vector>
#include <iostream>
#include "pixels.h"

int Qluma[8][8] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 36, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
};

int Qchroma[8][8] = {
        {17, 18, 24, 47, 99, 99, 99, 99},
        {18, 21, 26, 66, 99, 99, 99, 99},
        {24, 26, 56, 99, 99, 99, 99, 99},
        {47, 66, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99}
};

std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<int>>& v){
    if (v.empty()) return os;

    for (int i = 0; i < v.size(); i++){
        for (int j = 0; j < v[0].size(); j++){
            std::cout << v.at(i).at(j) << (j == v.size() - 1? "\n": " ");
        }
    }

    return os;
}

std::vector<std::vector<int>> Q(int R, int blockSize){
    std::vector<std::vector<int>> result(blockSize, std::vector<int>(blockSize));

    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            result[i][j] = 1 + (i + j) * R;
        }
    }

    return result;
}

std::vector<std::vector<int>> forwardQuantization(const std::vector<std::vector<int>>& block, int R, int blockSize){
    std::vector<std::vector<int>> result(blockSize, std::vector<int>(blockSize));
    std::vector<std::vector<int>> q = Q(R, blockSize);

    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            result[j][j] = (int)((int)(block[i][j]) / q[i][j]);
        }
    }

    return result;
}

std::vector<std::vector<int>> backwardQuantization(const std::vector<std::vector<int>>& block, int R, int blockSize){
    std::vector<std::vector<int>> result(blockSize, std::vector<int>(blockSize));
    std::vector<std::vector<int>> q = Q(R, blockSize);

    for (int i = 0; i < blockSize; i++){
        for (int j = 0; j < blockSize; j++){
            result[j][j] = clipping(block[i][j] * q[i][j]);
        }
    }

    return result;
}

#endif //JPEG_QUANTIZATION_H
