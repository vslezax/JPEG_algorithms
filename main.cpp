#include <iostream>
#include <fstream>
#include "Windows.h"
#include <vector>
#include <algorithm>

#include "pixels.h"
#include "YCbCr.h"
#include "DCT.h"
#include "quantization.h"

void fillVecByRand(std::vector<std::vector<int>>& v){
    srand(time(nullptr));

    for (int i = 0; i < v.size(); i++){
        for (int j = 0; j < v[0].size(); j++){
            v.at(i).at(j) = rand() % 256;
        }
    }
}

std::string createPath(std::string path, const std::string& message){
    path.insert(path.length() - 4, message);
    return path;
}

std::string intToString(int value) {
    std::string result;
    bool isNegative = value < 0;
    if (isNegative) value = -value;

    do {
        result += char(value % 10 + '0');
        value /= 10;
    } while (value);

    if (isNegative) result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

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

    std::vector<std::vector<unsigned char>> Y = returnY(inputPath, yPath);

    // 1.2.1 - 1.2.4
    std::vector<std::vector<unsigned char>> DCT_Y = DCTimage(Y, inputPath, dctPath, blockSize, false, 0);
    std::cout << "PSNR: " << PSNR(Y, DCT_Y) << std::endl;
    std::cout << std::endl;

    // 2.1 - 2.2
    for (int R = 1; R <= 10; R++){
        std::vector<std::vector<unsigned char>> DCT_Y_quantization = DCTimage(Y, inputPath, dctPath, blockSize, true, R);;
        std::cout << "R = " << R << ", PSNR: " << PSNR(Y, DCT_Y_quantization) << std::endl;
        if (R == 1 || R == 5 || R == 10){
            std::string rPath = createPath(inputPath, "_R" + intToString(R));
            Image input(inputPath);
            input.readData();

            Image r(rPath);
            r.writeChannel(input.fileHeader, input.infoHeader, DCT_Y_quantization);
        }
    }
}
