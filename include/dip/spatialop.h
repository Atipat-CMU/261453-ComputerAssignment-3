#ifndef SPARTIALOP_H
#define SPARTIALOP_H

#include <algorithm>
#include <cmath>

#include "../object/Image.h"
#include "../object/Kernel.h"

namespace dip {
    Image correlation(Image& image, Kernel& kernel){
        Image output(image.rows(), image.cols());

        for(int row = 0; row < image.rows(); row++){
            for(int col = 0; col < image.cols(); col++){
                double sum = 0;
                for(int i = 0; i < kernel.rows(); i++){
                    for(int k = 0; k < kernel.cols(); k++){
                        sum += image.getOutZero(row-kernel.c_x+i, col-kernel.c_y+k) * kernel.get(i,k);
                    }
                }
                output.set(row, col, (int)sum);
            }
        }

        return output;
    }

    Image convolution(Image& image, Kernel& kernel){
        Kernel flip_kernel(kernel.rows(), kernel.cols());

        for(int row = 0; row < kernel.rows(); row++){
            for(int col = 0; col < kernel.cols(); col++){
                int flip_row = kernel.rows() - row - 1;
                int flip_col = kernel.cols() - col - 1;
                flip_kernel.set(flip_row, flip_col, kernel.get(row, col));
            }
        }

        return correlation(image, flip_kernel);
    }

    Image medianFilter(Image& image, int size){
        Image output(image.rows(), image.cols());

        int c_x = floor(size/2.0);
        int c_y = floor(size/2.0);

        for(int row = 0; row < image.rows(); row++){
            for(int col = 0; col < image.cols(); col++){
                vector<int> values;
                for(int i = 0; i < size; i++){
                    for(int k = 0; k < size; k++){
                        values.push_back(image.getOutZero(row-c_x+i, col-c_y+k));
                    }
                }
                sort(values.begin(), values.end());
                output.set(row, col, values[ceil(values.size()/2.0)]);
            }
        }
        return output;
    }
}

#endif
