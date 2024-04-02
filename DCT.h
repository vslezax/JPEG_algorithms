#ifndef JPEG_DCT_H
#define JPEG_DCT_H

#include <vector>
#include <iostream>
#include <cmath>
#include "Image.h"
#include "PSNR.h"
#include "quantization.h"

#define pi M_PI

double C(int f, int N){
    return (f == 0? 1.0/N: 2.0/N);
}

std::vector<std::vector<int>> forwardDCT(const std::vector<std::vector<int>>& vector){
    int N = vector.size();
    if (N == 0){
        std::cerr << "forwardDCT() -> N == 0!" << std::endl;
        exit(-1);
    }
    std::vector<std::vector<int>> result(N, std::vector<int>(N, 0));

    for (int k = 0; k < N; k++){
        for (int l = 0; l < N; l++){
            double resultElement = 0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    resultElement += vector.at(i).at(j) *
                                     cos( (2*i+1)*pi/2/N*k ) *
                                     cos( (2*j+1)*pi/2/N*l );
                }
            }
            resultElement *= sqrt(C(k, N)) * sqrt(C(l, N));
            result.at(k).at(l) = (int)resultElement;
        }
    }

    return result;
}

std::vector<std::vector<int>> backwardDCT(const std::vector<std::vector<int>>& vector){
    int N = vector.size();
    if (N == 0){
        std::cerr << "backwardDCT() -> N == 0!" << std::endl;
        exit(-1);
    }
    std::vector<std::vector<int>> result(N, std::vector<int>(N, 0));

    for (int k = 0; k < N; k++){
        for (int l = 0; l < N; l++){
            double resultElement = 0;
            for (int i = 0; i < N; i++){
                for (int j = 0; j < N; j++){
                    double sum = 0;
                    sum += sqrt(C(i, N)) * sqrt(C(j, N));
                    sum *= vector.at(i).at(j);
                    sum *= cos((2 * k + 1) * pi / (2 * N) * i) *
                           cos((2 * l + 1) * pi / (2 * N) * j);
                    resultElement += sum;
                }
            }
            result.at(k).at(l) = (int)resultElement;
        }
    }

    return result;
}

std::vector<std::vector<unsigned char>> DCTimage(const std::vector<std::vector<unsigned char>>& Y, const std::string& inputPath, const std::string& outputPath, int blockSize){
    int H = Y.size();
    int W = Y[0].size();
    std::vector<std::vector<unsigned char>> outputData(H, std::vector<unsigned char>(W, 0));
    std::vector<std::vector<int>> block(blockSize, std::vector<int>(blockSize, 0));
    std::vector<std::vector<int>> DCTblock(blockSize, std::vector<int>(blockSize, 0));
    Image output(outputPath);
    Image input(inputPath);
    input.readData();

    // Tiling entire image
    for (int i = 0; i < H; i += blockSize){
        std::cout << "Working on tile [" << i << ", " << 0 << "] x [" << i + blockSize << ", " << W << "]..." << std::endl;
        int errors = 0;
        for (int j = 0; j < W; j += blockSize){
            // Forming block
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    block.at(k).at(l) = Y.at(i + k).at(j + l);
                }
            }

            // DCT
            std::vector<std::vector<int>> DCT = backwardDCT(forwardDCT(block));

            // Forming outputData + calculate Errors
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    outputData.at(i + k).at(j + l) = clipping(DCT.at(k).at(l));
                }
            }
        }
    }

    output.writeChannel(input.fileHeader, input.infoHeader, outputData);
    return outputData;
}

std::vector<std::vector<unsigned char>> DCTQuantizationImage(const std::vector<std::vector<unsigned char>>& Y, const std::string& inputPath, const std::string& outputPath, int blockSize, int R){
    int H = Y.size();
    int W = Y[0].size();
    std::vector<std::vector<unsigned char>> outputData(H, std::vector<unsigned char>(W, 0));
    std::vector<std::vector<int>> block(blockSize, std::vector<int>(blockSize, 0));
    std::vector<std::vector<int>> DCTblock(blockSize, std::vector<int>(blockSize, 0));
    Image output(outputPath);
    Image input(inputPath);
    input.readData();

    // Tiling entire image
    for (int i = 0; i < H; i += blockSize){
        std::cout << "Working on tile [" << i << ", " << 0 << "] x [" << i + blockSize << ", " << W << "]..." << std::endl;
        int errors = 0;
        for (int j = 0; j < W; j += blockSize){
            // Forming block
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    block.at(k).at(l) = Y.at(i + k).at(j + l);
                }
            }

            // DCT + Quantization
            std::vector<std::vector<int>> DCT = forwardDCT(block);
            std::vector<std::vector<int>> DCT_quantization = forwardQuantization(DCT, R, blockSize);
            std::vector<std::vector<int>> DCT_unquantization = backwardQuantization(DCT_quantization, R, blockSize);
            std::vector<std::vector<int>> unDCT_unquantization = backwardDCT(DCT_unquantization);

            // Forming outputData + calculate Errors
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    outputData.at(i + k).at(j + l) = clipping(unDCT_unquantization.at(k).at(l));
                }
            }
        }
    }
    return outputData;
}

#endif //JPEG_DCT_H
