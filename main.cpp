#include <iostream>
#include <fstream>
#include "Windows.h"
#include <vector>
#include <algorithm>

#include "pixels.h"
#include "YCbCr.h"
#include "DCT.h"
#include "quantization.h"
#include "Utils.h"

int main(int argc, char* argv[]){
    ///
    /// Discrete Cosine Transform, DCT:
    /// y = Tx
    ///
    // Example for N (size) = 8
    int N = 8;
    std::vector<std::vector<int>> exampleDCT(N, std::vector<int>(N, 0));
    fillVecByRand(exampleDCT);
    std::cout << "Original vec:\n" << exampleDCT << std::endl;

    std::vector<std::vector<int>> forwardDCTVector = forwardDCT(exampleDCT);
    std::cout << "Forward DCT vec:\n" << forwardDCTVector << std::endl;
    std::vector<std::vector<int>> backwardDCTVector = backwardDCT(forwardDCTVector);
    std::cout << "Backward DCT vec:\n" << backwardDCTVector << std::endl;

    int errors = 0;
    int maxError = 0;
    for (int i = 0; i < exampleDCT.size(); i++){
        for (int j = 0; j < exampleDCT[0].size(); j++){
            if (exampleDCT.at(i).at(j) != backwardDCTVector.at(i).at(j)) errors++;
            if (maxError < abs(exampleDCT.at(i).at(j) - backwardDCTVector.at(i).at(j)))
                maxError = abs(exampleDCT.at(i).at(j) - backwardDCTVector.at(i).at(j));
        }
    }
    std::cout << errors << " errors in example (" <<  100.0 * errors / N / N << "%) with max amplitude " << maxError << std::endl;


    ///
    /// Start
    ///
    std::string inputPath;
    int blockSize;

    std::cout << "Input image path: ";
    std::cin >> inputPath;
    std::cout << "Block size: ";
    std::cin >> blockSize;
    std::string yPath = createPath(inputPath, "_Y");
    std::string dctPath = createPath(inputPath, "_DCT");
    Image input(inputPath);
    input.readData();

    std::vector<std::vector<unsigned char>> Y = returnY(inputPath, yPath);

    // 1.2.1 - 1.2.4
    std::vector<std::vector<int>> DCT_Y = DCTimage(unclippingVector(Y), blockSize, true, true);
    std::vector<std::vector<int>> unDCT_Y = DCTimage(DCT_Y, blockSize, false, true);
    std::cout << "PSNR: " << PSNR(Y, clippingVector(unDCT_Y)) << std::endl;

    Image DCTImage(dctPath);
    std::vector<std::vector<unsigned char>> unDCT_Y_clipping = clippingVector(unDCT_Y);
    DCTImage.writeChannel(input.fileHeader, input.infoHeader, unDCT_Y_clipping);
    std::cout << std::endl;


    // 2.1 - 2.2
    for (int R = 0; R <= 10; R++){
        std::vector<std::vector<int>> quantization_Y = QuantizateImage(DCT_Y, blockSize, R, true);
        std::vector<std::vector<int>> unQuantization_Y = QuantizateImage(quantization_Y, blockSize, R, false);
        std::vector<std::vector<int>> unQuantization_unDCT_Y = DCTimage(unQuantization_Y, blockSize, false, false);
        std::vector<std::vector<unsigned char>> unQuantization_unDCT_Y_clipping = clippingVector(unQuantization_unDCT_Y);
        std::cout << "R = " << R << ", PSNR: " << PSNR(Y, unQuantization_unDCT_Y_clipping) << std::endl;

        std::string rPath = createPath(inputPath, "_R" + intToString(R));
        Image r(rPath);
        r.writeChannel(input.fileHeader, input.infoHeader, unQuantization_unDCT_Y_clipping);
    }
}
