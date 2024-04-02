#ifndef JPEG_DCT_H
#define JPEG_DCT_H

#include <vector>
#include <iostream>
#include <cmath>
#include "Image.h"

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

void DCTimage(const std::string& inputPath, const std::string& yPath, const std::string& outputPath, int blockSize){
    if (blockSize <= 0) return;
    Image src(inputPath);
    Image yImage(yPath);
    Image output(outputPath);
    src.readData();
    int H = src.H;
    int W = src.W;

    // Reading data
    std::vector<std::vector<YCbCrPixel>> srcDataFull = RGBtoYCbCr(src.data, H, W);
    std::vector<std::vector<int>> srcData(H, std::vector<int>(W, 0));
    // Data -> Y
    std::vector<std::vector<unsigned char>> Y(H, std::vector<unsigned char>(W, 0));
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            srcData.at(i).at(j) = srcDataFull.at(i).at(j).Y;
            Y.at(i).at(j) = srcData.at(i).at(j);
        }
    }
    // Creating Y image
    yImage.writeChannel(src.fileHeader, src.infoHeader, Y);
    std::vector<std::vector<unsigned char>> outputData(H, std::vector<unsigned char>(W, 0));


    // Tiling entire image
    int errorsFull = 0;
    std::vector<std::vector<int>> block(blockSize, std::vector<int>(blockSize, 0));
    std::vector<std::vector<int>> DCTblock(blockSize, std::vector<int>(blockSize, 0));
    for (int i = 0; i < src.H; i += blockSize){
        std::cout << "Working on tile [" << i << ", " << 0 << "] x [" << i + blockSize << ", " << src.W << "]...";
        int errors = 0;
        for (int j = 0; j < src.W; j += blockSize){
            // Forming block
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    block.at(k).at(l) = srcData.at(i + k).at(j + l);
                }
            }

            // DCT
            std::vector<std::vector<int>> DCTB = backwardDCT(forwardDCT(block));

            // Forming outputData + calculate Errors
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    outputData.at(i + k).at(j + l) = clipping(DCTB.at(k).at(l));
                    if (Y.at(k).at(l) != outputData.at(k).at(l)) errors++;
                }
            }
            errorsFull += errors;
        }
        std::cout << "    Done. Errors: " << errors << std::endl;
    }
    std::cout << "Sum Errors: " << errorsFull << std::endl;

    output.writeChannel(src.fileHeader, src.infoHeader, outputData);
}

///
/// NOT WORKING (IDK WHY)
///
void DCTimageRGB(const std::string& inputPath, const std::string& outputPath, int blockSize){
    if (blockSize <= 0) return;
    Image src(inputPath);
    src.readData();
    std::vector<std::vector<RGBPixel>> srcData = src.data;
    std::vector<std::vector<RGBPixel>> outputData = src.data;

    Image output(outputPath);

    int errorsRfull = 0;
    int errorsGfull = 0;
    int errorsBfull = 0;
    std::vector<std::vector<RGBPixel>> RGBblock(blockSize, std::vector<RGBPixel>(blockSize, {0, 0, 0}));
    std::vector<std::vector<RGBPixel>> DCTRGBblock(blockSize, std::vector<RGBPixel>(blockSize, {0, 0, 0}));
    for (int i = 0; i < src.H; i += blockSize){
        std::cout << "Working on tile [" << i << ", " << 0 << "] x [" << i + blockSize << ", " << src.W << "]...";
        int errorsR = 0;
        int errorsG = 0;
        int errorsB = 0;
        for (int j = 0; j < src.W; j += blockSize){
            // Forming PIXELS block
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    RGBblock.at(k).at(l) = src.data.at(i + k).at(j + l);
                }
            }

            // R channel
            std::vector<std::vector<int>> R(blockSize, std::vector<int>(blockSize, 0));
            for (int k = 0; k < blockSize; k++) {
                for (int l = 0; l < blockSize; l++) {
                    R.at(k).at(l) = RGBblock.at(k).at(l).R;
                }
            }
            std::vector<std::vector<int>> DCTR = backwardDCT(forwardDCT(R));

            // G channel
            std::vector<std::vector<int>> G(blockSize, std::vector<int>(blockSize, 0));
            for (int k = 0; k < blockSize; k++) {
                for (int l = 0; l < blockSize; l++) {
                    G.at(k).at(l) = RGBblock.at(k).at(l).G;
                }
            }
            std::vector<std::vector<int>> DCTG = backwardDCT(forwardDCT(G));

            // B channel
            std::vector<std::vector<int>> B(blockSize, std::vector<int>(blockSize, 0));
            for (int k = 0; k < blockSize; k++) {
                for (int l = 0; l < blockSize; l++) {
                    B.at(k).at(l) = RGBblock.at(k).at(l).B;
                }
            }
            std::vector<std::vector<int>> DCTB = backwardDCT(forwardDCT(B));

            // Forming DCTRGBblock
            for (int k = 0; k < blockSize; k++) {
                for (int l = 0; l < blockSize; l++) {
                    DCTRGBblock.at(k).at(l) = {R.at(k).at(l), B.at(k).at(l), G.at(k).at(l)};
                }
            }

            // Forming outputData + calculate Errors
            for (int k = 0; k < blockSize; k++){
                for (int l = 0; l < blockSize; l++){
                    outputData.at(i + k).at(j + l) = DCTRGBblock.at(k).at(l);

                    if (DCTRGBblock.at(k).at(l).R != RGBblock.at(k).at(l).R) errorsR++;
                    if (DCTRGBblock.at(k).at(l).G != RGBblock.at(k).at(l).G) errorsG++;
                    if (DCTRGBblock.at(k).at(l).B != RGBblock.at(k).at(l).B) errorsB++;
                }
            }
            errorsRfull += errorsR;
            errorsGfull += errorsG;
            errorsBfull += errorsB;
        }
        std::cout << "    Done. Errors: (" << errorsR << ", " << errorsB << ", " << errorsB << ")" << std::endl;
    }
    std::cout << "Sum Errors: (" << errorsRfull << ", " << errorsBfull << ", " << errorsBfull << ")" << std::endl;

    output.writeImage(src.fileHeader, src.infoHeader, outputData);
}

#endif //JPEG_DCT_H
