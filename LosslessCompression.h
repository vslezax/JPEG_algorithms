#ifndef JPEG_LOSSLESSCOMPRESSION_H
#define JPEG_LOSSLESSCOMPRESSION_H

#include <vector>
#include <map>
#include <fstream>

void printHistogram(const std::vector<int>& data, const std::string& path){
    std::map<int, int> freq;
    for (int value : data){
        freq[value]++;
    }

    std::ofstream file(path);
    if (!file.is_open()){
        std::cout << "printHistogram() -> file not opened!" << std::endl;
        exit(-1);
    }

    for (const auto& pair : freq){
        file << pair.first << " " << pair.second << std::endl;
    }

    std::cout << path << " created." << std::endl;
}

std::vector<int> calculateDCdiff(const std::vector<int>& DC){
    std::vector<int> result;

    for (int i = 0; i < DC.size(); i++){
        int diff;
        if (i != 0) diff = DC[i] - DC[i - 1];
        else diff = DC[i];
        result.push_back(diff);
    }
    return result;
}

std::vector<int> exportDC(const std::vector<std::vector<unsigned char>>& Y, int blockSize){
    int H = Y.size();
    int W = Y[0].size();
    std::vector<int> result;

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

bool operator==(const std::pair<int, int>& a, const std::pair<int, int>& b){
    return a.first == b.first && a.second == b.second;
}

double entropy(const std::vector<std::pair<int, int>>& a){
    double result = 0;
    std::map<std::pair<int, int>, int> map;

    for (auto value : a) {
        if (map.find(value) != map.end()) map.find(value)->second++;
        else map.emplace(value, 1);
    }

    for (auto i: map) {
        double probability = (double)(map.find(i.first)->second) / a.size();
        result -= probability * log2(probability);
    }

    return result;
}

#endif //JPEG_LOSSLESSCOMPRESSION_H
