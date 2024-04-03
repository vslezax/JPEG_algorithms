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
    std::cout << "Block size (default = 8): ";
    std::cin >> blockSize;
    std::string dctPath = createPath(inputPath, "_DCT");
    Image input(inputPath);
    input.readData();

    std::cout << "Choose channel: Y (=y), Cb (=b), Cr (=r)? " << std::endl;
    char channel;
    std::cin >> channel;
    std::string channelPath(1, channel);
    channelPath += "\\";

    std::vector<std::vector<unsigned char>> channelData = returnChannel(inputPath, channel);

    // 1.2.1 - 1.2.43
    std::vector<std::vector<int>> DCT_Y = DCTimage(unclippingVector(channelData), forwardDCT, blockSize, true);
    std::vector<std::vector<int>> unDCT_Y = DCTimage(DCT_Y, backwardDCT, blockSize, true);
    std::cout << "PSNR: " << PSNR(channelData, clippingVector(unDCT_Y)) << std::endl;

    Image DCTImage(dctPath);
    std::vector<std::vector<unsigned char>> unDCT_Y_clipping = clippingVector(unDCT_Y);
    DCTImage.writeChannel(input.fileHeader, input.infoHeader, unDCT_Y_clipping);
    std::cout << std::endl;

    double originalSize = 8 * input.H * input.W;
    std::string directory = createFreqFolder(inputPath) + channelPath;
    std::ofstream file(directory + "PSNR_R.txt");
    if (!file.is_open()){
        std::cerr << "main() -> file isn't open" << std::endl;
        exit(-1);
    }
    for (int R = 0; R <= 10; R++){
        // 2.1 - 2.2
        std::vector<std::vector<int>> quantization_Y = QuantizateImage(DCT_Y, forwardQuantization, blockSize, R);
        std::vector<std::vector<int>> unQuantization_Y = QuantizateImage(quantization_Y, backwardQuantization, blockSize, R);
        std::vector<std::vector<int>> unQuantization_unDCT_Y = DCTimage(unQuantization_Y, backwardDCT, blockSize, false);
        std::vector<std::vector<unsigned char>> unQuantization_unDCT_Y_clipping = clippingVector(unQuantization_unDCT_Y);
        double psnr = PSNR(channelData, unQuantization_unDCT_Y_clipping);
        std::cout << "R = " << R << ", PSNR: " << psnr << std::endl;
        file << R << " " << psnr << std::endl;
        std::string rPath = createPath(inputPath, "_R" + intToString(R) + "_" + std::string(1, channel));
        Image r(rPath);
        if (R == 1 || R == 5 || R == 10) r.writeChannel(input.fileHeader, input.infoHeader, unQuantization_unDCT_Y_clipping);

        // 3.1 + 3.2
        std::vector<int> DC = exportDC(unQuantization_unDCT_Y_clipping, blockSize);
        std::vector<int> dDC = calculateDCdiff(DC);
        std::vector<int> BC_dDC = calculateBitCategory(dDC); // (BC_dDC, MG) = (BC_dDC, DC)
        printHistogram(DC, directory + "DC_R" + intToString(R) + ".txt");
        printHistogram(dDC, directory + "DC_diff_R" + intToString(R) + ".txt");
        std::cout << "DC entropy: " << entropy(DC) << std::endl;
        std::cout << "dDC entropy: " << entropy(dDC) << std::endl;

        // 3.3
        // AC compress
        std::vector<std::pair<int, int>> AC_array;
        std::vector<int> AC_array_MG;
        for (int i = 0; i < input.H; i += blockSize){
            for (int j = 0; j < input.W; j += blockSize){
                std::vector<std::pair<int, int>> compressedAC = compressAC(unQuantization_unDCT_Y_clipping, i, j, blockSize);
                for (auto ac: compressedAC){
                    AC_array.push_back(ac);
                    AC_array_MG.push_back(ac.second);
                }
            }
        }
        std::vector<int> BC_AC = calculateBitCategory(AC_array_MG);

        double BCsum = 0;
        for (auto i : BC_dDC) BCsum += i;
        double BC_ACsum = 0;
        for (auto i : BC_AC) BC_ACsum += i;
        double compress = entropy(BC_dDC) * DC.size() + BCsum + entropy(AC_array) * AC_array.size() + BC_ACsum;
        std::cout << "Original size: " << originalSize << std::endl;
        std::cout << "Compress size: " << compress << std::endl;
        std::cout << "Percentage: " << compress / (originalSize / 100.0) << "%" << std::endl;
        std::cout << std::endl;
    }
    file.close();
}
