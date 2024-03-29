#ifndef MORPHOLOGICAL_H
#define MORPHOLOGICAL_H

#include <algorithm>
#include <cmath>

#include "../object/Image.h"
#include "../object/Kernel.h"

namespace dip {
    Image erosion(Image& image, Kernel& kernel){
        Image output(image.rows(), image.cols());

        bool keep = true;

        for(int row = 0; row < image.rows(); row++){
            for(int col = 0; col < image.cols(); col++){
                bool keep = true;
                for(int i = 0; i < kernel.rows(); i++){
                    for(int k = 0; k < kernel.cols(); k++){
                        if(kernel.get(i,k) == 1){
                            if(image.getOutZero(row-kernel.c_x+i, col-kernel.c_y+k) == 0){
                                keep = false;
                                break;
                            }
                        }
                    }
                    if(!keep) break;
                }
                if(keep) output.set(row, col, image.get(row, col));
            }
        }
        return output;
    }
}

#endif
