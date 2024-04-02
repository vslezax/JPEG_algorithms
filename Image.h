#ifndef JPEG_IMAGE_H
#define JPEG_IMAGE_H

#include <vector>
#include <string>
#include <fstream>

#include "pixels.h"

class Image {
public:
    int H;
    int W;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::vector<std::vector<RGBPixel>> data;
    std::string path;
    Image(const std::string& inputPath){
        path = inputPath;
    }
    void readData(){
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open()){
            std::cerr << "Image -> " << path << " wasn't open!";
            exit(-1);
        }

        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
        W = infoHeader.biWidth;
        H = infoHeader.biHeight;
        std::vector<RGBPixel> dataVector(W * H, new RGBPixel(0, 0, 0));
        file.read(reinterpret_cast<char*>(dataVector.data()), infoHeader.biSizeImage);

        // 1xSize vector ---> HxW vector
        data = std::vector<std::vector<RGBPixel>>(H, std::vector<RGBPixel>(W, new RGBPixel(0, 0, 0)));
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                data.at(i).at(j) = dataVector.at(i * W + j);
            }
        }

        file.close();
    }
    void writeImage(BITMAPFILEHEADER fileHeaderNew, BITMAPINFOHEADER infoHeaderNew, std::vector<std::vector<RGBPixel>>& dataNew){
        H = dataNew.size();
        W = dataNew[0].size();

        std::ofstream file(path, std::ios::binary | std::ios::out);
        file.write(reinterpret_cast<char*>(&fileHeaderNew), sizeof(fileHeaderNew));
        file.write(reinterpret_cast<char*>(&infoHeaderNew), sizeof(infoHeaderNew));
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                file.write(reinterpret_cast<char*>(&dataNew.at(i).at(j).R), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&dataNew.at(i).at(j).G), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&dataNew.at(i).at(j).B), sizeof(unsigned char));
            }
        }
        file.close();
        std::cout << path + " created." << std::endl;
    }
    void writeChannel(BITMAPFILEHEADER fileHeaderNew, BITMAPINFOHEADER infoHeaderNew, std::vector<std::vector<unsigned char>>& dataNew){
        H = dataNew.size();
        W = dataNew[0].size();

        std::ofstream file(path, std::ios::binary | std::ios::out);
        file.write(reinterpret_cast<char*>(&fileHeaderNew), sizeof(fileHeaderNew));
        file.write(reinterpret_cast<char*>(&infoHeaderNew), sizeof(infoHeaderNew));
        for (int i = 0; i < H; i++){
            for (int j = 0; j < W; j++){
                file.write(reinterpret_cast<char*>(&dataNew.at(i).at(j)), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&dataNew.at(i).at(j)), sizeof(unsigned char));
                file.write(reinterpret_cast<char*>(&dataNew.at(i).at(j)), sizeof(unsigned char));
            }
        }
        file.close();
        std::cout << path + " created." << std::endl;
    }
    ~Image(){
        path.clear();
        data.clear();
    }
};


#endif //JPEG_IMAGE_H
