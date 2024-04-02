#include <iostream>
#include <fstream>
#include "Windows.h"
#include <vector>

#include "pixels.h"
#include "YCbCr.h"
#include "DCT.h"

std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<int>>& v){
    if (v.empty()) return os;

    for (int i = 0; i < v.size(); i++){
        for (int j = 0; j < v[0].size(); j++){
            std::cout << v.at(i).at(j) << (j == v.size() - 1? "\n": " ");
        }
    }

    return os;
}

void fillVecByRand(std::vector<std::vector<int>>& v){
    srand(time(nullptr));

    for (int i = 0; i < v.size(); i++){
        for (int j = 0; j < v[0].size(); j++){
            v.at(i).at(j) = rand() % 256;
        }
    }
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

    // 1.2.1 - 1.2.4
    while (true){
        std::cout << "Start DCT for image? 'n' = exit" << std::endl;
        char ch;
        std::cin >> ch;
        if (ch == 'n') break;

        std::string inputPath;
        std::string yPath;
        std::string DCTPath;
        std::string outputPath;
        int blockSize;

        std::cout << "Input image path: ";
        std::cin >> inputPath;
        std::cout << "Only Y image path: ";
        std::cin >> yPath;
        std::cout << "Output image path: ";
        std::cin >> outputPath;
        std::cout << "Block size: ";
        std::cin >> blockSize;

        DCTimage(inputPath, yPath, outputPath, blockSize);
        //DCTimageRGB(inputPath, outputPath, blockSize);
    }
}
