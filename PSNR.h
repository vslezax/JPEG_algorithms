#ifndef JPEG_PSNR_H
#define JPEG_PSNR_H

#include <vector>
#include <cmath>

double PSNR(const std::vector<std::vector<unsigned char>>& a, const std::vector<std::vector<unsigned char>>& b){
    int a_a = a.size();
    int a_b = a[0].size();
    int b_a = b.size();
    int b_b = b[0].size();
    if (a_a != b_a || a_b != b_b){
        std::cout << "PSNR() -> a.size != b.size:" << a_a << "x" << a_b << " <-> " << b_a << "x" << b_b << std::endl;
        exit(-1);
    }

    double result = pow(2, 8) - 1;
    result = pow(result, 2);
    result *= a.size() * a[0].size();

    double tmp = 0;
    for (int i = 0; i < a.size(); i++){
        for (int j = 0; j < a[0].size(); j++){
            tmp += pow(a[i][j] - b[i][j], 2);
        }
    }

    result /= tmp;

    return 10 * log10(result);
}

#endif //JPEG_PSNR_H
