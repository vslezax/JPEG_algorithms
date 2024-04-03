#ifndef JPEG_DCT_H
#define JPEG_DCT_H

#include <vector>
#include <iostream>
#include <cmath>
#include "Image.h"
#include "PSNR.h"
#include "quantization.h"

#define pi M_PI

using data = std::vector<std::vector<int>>;

double C(int f, int N){
    return (f == 0? 1.0/N: 2.0/N);
}

data forwardDCT(const data& vector){
    int N = vector.size();
    if (N == 0){
        std::cerr << "forwardDCT() -> N == 0!" << std::endl;
        exit(-1);
    }
    data result(N, std::vector<int>(N, 0));

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

data backwardDCT(const data& vector){
    int N = vector.size();
    if (N == 0){
        std::cerr << "backwardDCT() -> N == 0!" << std::endl;
        exit(-1);
    }
    data result(N, std::vector<int>(N, 0));

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

data DCTimage(const data& Y, data(*func)(const data&), int blockSize, bool log){
    int H = Y.size();
    int W = Y[0].size();
    data outputData(H, std::vector<int>(W, 0));
    data block(blockSize, std::vector<int>(blockSize, 0));

    // Tiling entire image
    for (int i = 0; i < H; i += blockSize){
        if (log) std::cout << "Working on tile [" << i << ", " << 0 << "] x [" << i + blockSize << ", " << W << "]..." << std::endl;
        int errors = 0;
        for (int j = 0; j < W; j += blockSize){
            // Forming block
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    block.at(k).at(l) = Y.at(i + k).at(j + l);
                }
            }

            // DCT
            data DCT = func(block);

            // Forming outputData
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    outputData.at(i + k).at(j + l) = DCT.at(k).at(l);
                }
            }
        }
    }
    return outputData;
}


#endif //JPEG_DCT_H
