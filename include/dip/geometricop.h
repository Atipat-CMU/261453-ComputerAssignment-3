#ifndef GEOMETRICOP_H
#define GEOMETRICOP_H

#include <cmath>

#include "../object/Image.h"

namespace dip {
    int BilinearInterpolation(Image& image, double x, double y){
        int f00 = image.getOutZero(floor(x), floor(y));
        int f01 = image.getOutZero(floor(x), ceil(y));
        int f10 = image.getOutZero(ceil(x), floor(y));
        int f11 = image.getOutZero(ceil(x), ceil(y));

        double a = f10-f00;
        double b = f01-f00;
        double c = f11+f00-f01-f10;
        double d = f00;

        return int(abs(a*x + b*y + c*x*y + d)); 
    }

    int NearestInterpolation(Image& image, double x, double y){
        return image.getOutZero(round(x), round(y));
    }

    Image rotate(Image& ori_image, double deg){
        double r = deg*3.14159/180;

        int x_n = ori_image.rows() - 1;
        int y_n = ori_image.cols() - 1;

        vector<Pixel> conner_pixel(4);
        
        Pixel F00 = Pixel(0,0);
        conner_pixel.push_back(F00);
        Pixel F0N = Pixel(ceil(-sin(r)*y_n),ceil(cos(r)*y_n));
        conner_pixel.push_back(F0N);
        Pixel FN0 = Pixel(ceil(cos(r)*x_n),ceil(sin(r)*x_n));
        conner_pixel.push_back(FN0);
        Pixel FNN = Pixel(ceil(cos(r)*x_n-sin(r)*y_n),ceil(sin(r)*x_n+cos(r)*y_n));
        conner_pixel.push_back(FNN);

        // leftmost pixel
        Pixel leftmost = F00;
        for (auto& pixel : conner_pixel) {
            if(pixel.y() < leftmost.y()){
                leftmost = pixel;
            }
        }

        // rightmost pixel
        Pixel rightmost = F00;
        for (auto& pixel : conner_pixel) {
            if(pixel.y() > rightmost.y()){
                rightmost = pixel;
            }
        }

        // topmost pixel
        Pixel topmost = F00;
        for (auto& pixel : conner_pixel) {
            if(pixel.x() < topmost.x()){
                topmost = pixel;
            }
        }

        // bottommost pixel
        Pixel bottommost = F00;
        for (auto& pixel : conner_pixel) {
            if(pixel.x() > bottommost.x()){
                bottommost = pixel;
            }
        }

        int F00_new_x = topmost.x();
        int F00_new_y = leftmost.y();
        int numRows = bottommost.x() - topmost.x() + 1;
        int numCols = rightmost.y() - leftmost.y() + 1;

        Image new_image(numRows, numCols);

        double cos_v = cos(r);
        double sin_v = sin(r);

        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                int x_ = row + F00_new_x;
                int y_ = col + F00_new_y;
                double x = cos_v*x_+sin_v*y_;
                double y = -sin_v*x_+cos_v*y_;
                new_image.set(row, col, NearestInterpolation(ori_image, x, y));
            }
        }

        return new_image;
    }

    Image downSample(Image& ori_image, int value){
        int numRows = round(ori_image.rows()/2);
        int numCols = round(ori_image.cols()/2);

        Image new_image(numRows, numCols);

        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                double x = row*value;
                double y = col*value;
                new_image.set(row, col, NearestInterpolation(ori_image, x, y));
            }
        }

        return new_image;
    }
}

#endif
