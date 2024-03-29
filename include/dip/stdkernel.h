#ifndef STD_KERNEL_H
#define STD_KERNEL_H

#include <cmath>

#include "../object/Kernel.h"

namespace dip {
    Kernel GaussianKernel(int size, double std_dev){
        Kernel kernel(size, size);

        int c_x = floor(size/2.0);
        int c_y = floor(size/2.0);
        double norm = 1/(2*M_PI*pow(std_dev,2));
        double sum = 0;

        for(int row = 0; row < size; row++){
            for(int col = 0; col < size; col++){
                kernel.set(row, col, norm*exp(-((pow(row-c_x,2)+pow(col-c_y,2))/(2*pow(std_dev,2)))));
                sum += kernel.get(row, col);
            }
        }

        for(int row = 0; row < size; row++){
            for(int col = 0; col < size; col++){
                kernel.set(row, col, kernel.get(row, col)/sum);
            }
        }

        return kernel;
    }
}

#endif
