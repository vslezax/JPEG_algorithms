#include <iostream>
#include <vector>
#include <algorithm>

#include "YCbCr.h"
#include "DCT.h"
#include "quantization.h"
#include "Utils.h"
#include "LosslessCompression.h"

int main(){
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



    for (int R = 0; R <= 10; R++){
        // 2.1 - 2.2
        std::vector<std::vector<int>> quantization_Y = QuantizateImage(DCT_Y, blockSize, R, true);
        std::vector<std::vector<int>> unQuantization_Y = QuantizateImage(quantization_Y, blockSize, R, false);
        std::vector<std::vector<int>> unQuantization_unDCT_Y = DCTimage(unQuantization_Y, blockSize, false, false);
        std::vector<std::vector<unsigned char>> unQuantization_unDCT_Y_clipping = clippingVector(unQuantization_unDCT_Y);
        std::cout << "R = " << R << ", PSNR: " << PSNR(Y, unQuantization_unDCT_Y_clipping) << std::endl;
        std::string rPath = createPath(inputPath, "_R" + intToString(R));
        Image r(rPath);
        r.writeChannel(input.fileHeader, input.infoHeader, unQuantization_unDCT_Y_clipping);

        // 3.1 + 3.2
        std::string directory = createFreqFolder(inputPath);
        std::vector<int> DC = exportDC(unQuantization_unDCT_Y_clipping, blockSize);
        std::vector<int> DC_diff = calculateDCdiff(DC);
        std::vector<int> BC = calculateBitCategory(DC_diff); // (BC, MG) = (BC, DC)
        printHistogram(DC, directory + "DC_" + intToString(R) + ".txt");
        printHistogram(DC_diff, directory + "DC_diff_" + intToString(R) + ".txt");
        std::cout << "DC entropy: " << entropy(DC) << std::endl;
        std::cout << "DC_diff entropy: " << entropy(DC_diff) << std::endl;

        // 3.3
        // AC compress
        std::vector<std::pair<int, int>> AC_array;
        for (int i = 0; i < input.H; i += blockSize){
            for (int j = 0; j < input.W; j += blockSize){
                std::vector<std::pair<int, int>> compressedAC = compressAC(unQuantization_unDCT_Y_clipping, i, j, blockSize);
                for (auto ac: compressedAC) AC_array.push_back(ac);
            }
        }

        int BCsum = 0;
        for (auto i : BC) BCsum += i;
        double compressionEntropy = entropy(BC)*DC.size() + entropy(AC_array)*AC_array.size()
    }
}
