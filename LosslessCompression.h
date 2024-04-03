#ifndef JPEG_LOSSLESSCOMPRESSION_H
#define JPEG_LOSSLESSCOMPRESSION_H

#include <vector>
#include <map>
#include <fstream>

void printHistogram(const std::vector<int>& data, const std::string& path){
    std::map<int, int> freq;
    for (int i = 0; i < data.size(); i++){
        if (freq.find(data[i]) != freq.end()){
            freq.find(data[i])->second++;
        }
        else freq.insert(std::pair<int, int>(data[i], 1));
    }

    std::ofstream file(path);
    if (!file.is_open()){
        std::cout << "printHistogram() -> file not opened!" << std::endl;
        exit(-1);
    }
    for (int i = 0; i < 256; i++){
        file << i << " " << (freq.find(i) == freq.end()? 0: freq.find(i)->second) << std::endl;
    }
    std::cout << path << " created." << std::endl;
}

std::vector<int> calculateDCdiff(const std::vector<int>& DC){
    std::vector<int> result;

    // Tiling entire image
    for (int i = 0; i < DC.size(); i++){
        result.push_back(i != 0? DC[i] - DC[i - 1]: DC[i]);
    }
    return result;
}

std::vector<int> exportDC(const std::vector<std::vector<unsigned char>>& Y, int blockSize){
    int H = Y.size();
    int W = Y[0].size();
    std::vector<int> result;

    // Tiling entire image
    for (int i = 0; i < H; i += blockSize){
        for (int j = 0; j < W; j += blockSize){
            result.push_back(Y[i][j]);
        }
    }
    return result;
}

int bitCategory(int a){
    return std::ceil(log2(abs(a)+1));
}

std::vector<int> calculateBitCategory(const std::vector<int>& DC){
    std::vector<int> result;
    for (int i = 0; i < DC.size(); i++) result.push_back(bitCategory(DC[i]));
    return result;
}

std::vector<std::pair<int, int>> compressAC(const std::vector<std::vector<unsigned char>>& data, int i, int j, int blockSize){
    std::vector<int> dataVector;

    // Zigzag
    int rows = blockSize;
    int cols = blockSize;
    int row = 0, col = 1;
    bool up = true;
    for (int diag = 0; diag < rows + cols - 1; diag++) {
        if (up) {
            while (row >= 0 && col < cols) {
                dataVector.push_back(data[i + row][j + col]);
                row--;
                col++;
            }
            if (col == cols) {
                row += 2;
                col--;
            } else {
                row = 0;
            }
            up = false;
        } else {
            while (col >= 0 && row < rows) {
                dataVector.push_back(data[i + row][j + col]);
                row++;
                col--;
            }
            if (row == rows) {
                row--;
                col += 2;
            } else {
                col = 0;
            }
            up = true;
        }
    }

    // AC -> (r, l)
    std::vector<std::pair<int, int>> result;
    int n = 1;
    int tmp = dataVector[0];
    for (int i = 1; i < dataVector.size(); i++){
        if (dataVector[i] == tmp) n++;
        else{
            result.emplace_back(n, dataVector[i]);
            n = 1;
            i++;
            tmp = dataVector[i];
        }
    }

    return result;
}

double entropy(const std::vector<std::pair<int, int>>& a){
    double result = 0;
    std::vector<int> counts(256 * 256, 0); // Assuming pairs are in the range [0, 255]
    int totalPairs = a.size();

    for (const auto& pair : a) {
        int first_value = pair.first;
        int second_value = pair.second;
        int index = first_value * 256 + second_value; // Assuming pairs are in the range [0, 255]
        counts[index]++;
    }

    for (int i = 0; i < 256 * 256; i++) { // Assuming pairs are in the range [0, 255]
        if (counts[i] > 0) {
            double probability = static_cast<double>(counts[i]) / totalPairs;
            result -= probability * log2(probability);
        }
    }

    return result;
}

#endif //JPEG_LOSSLESSCOMPRESSION_H
