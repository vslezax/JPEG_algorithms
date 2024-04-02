#ifndef JPEG_UTILS_H
#define JPEG_UTILS_H

#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

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

std::string createPath(std::string path, const std::string& message){
    path.insert(path.length() - 4, message);
    return path;
}

std::string createFreqFolder(std::string path){
    while (path[path.length() - 1] != '\\') path.pop_back();
    path.insert(path.length(), "Freq\\");
    return path;
}

template<class T>
double entropy(const std::vector<T>& a){
    double result = 0;
    std::vector<int> counts(256, 0);
    int totalPixels = a.size();

    for (T pixel : a) {
        counts[pixel]++;
    }

    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            double probability = static_cast<double>(counts[i]) / totalPixels;
            result -= probability * log2(probability);
        }
    }

    return result;
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

unsigned char clipping(double value){
    if (value < 0) return 0;
    if (value > UCHAR_MAX) return UCHAR_MAX;
    return (char)value;
}

std::vector<std::vector<unsigned char>> clippingVector(const std::vector<std::vector<int>>& v){
    std::vector<std::vector<unsigned char>> result(v.size(), std::vector<unsigned char>(v[0].size()));
    for (int i = 0; i < v.size(); i++){
        for (int j = 0; j < v[0].size(); j++){
            result[i][j] = clipping(v[i][j]);
        }
    }
    return result;
}

std::vector<std::vector<int>> unclippingVector(const std::vector<std::vector<unsigned char>>& v){
    std::vector<std::vector<int>> result(v.size(), std::vector<int>(v[0].size()));
    for (int i = 0; i < v.size(); i++){
        for (int j = 0; j < v[0].size(); j++){
            result[i][j] = clipping(v[i][j]);
        }
    }
    return result;
}

#endif //JPEG_UTILS_H
