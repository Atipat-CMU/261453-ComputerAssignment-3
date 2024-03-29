#ifndef POINTOP_H
#define POINTOP_H

#include <map>

#include "../object/Image.h"

namespace dip {
    Image poperation(Image image, int (*fun)(int)){
        Image new_image(image.rows(), image.cols());

        for(int row = 0; row < image.rows(); row++){
            for(int col = 0; col < image.cols(); col++){
                int d = fun(image.get(row, col));
                if(d < 0) d = 0;
                if(d > 255) d = 255;
                new_image.set(row, col, d);
            }
        }

        return new_image;
    }

    Image poperation(Image image, int (*fun)(int, int)){
        Image new_image(image.rows(), image.cols());

        for(int row = 0; row < image.rows(); row++){
            for(int col = 0; col < image.cols(); col++){
                int d = fun(image.get(row, col), image.max());
                if(d < 0) d = 0;
                if(d > 255) d = 255;
                new_image.set(row, col, d);
            }
        }

        return new_image;
    }

    Image equalizeHist(Image image, int min, int max){
        map<int,int> f_d;

        float cumul = 0;

        for(int D = 0; D <= max; D++){
            float p = image.getHistFreq(D)/float(image.area());
            float Dnew = (cumul + p) * max;
            cumul += p;
            if(D >= min && D <= max) f_d[D] = Dnew;
            else f_d[D] = D;
        }

        Image new_image(image.rows(), image.cols());

        for(int row = 0; row < image.rows(); row++){
            for(int col = 0; col < image.cols(); col++){
                new_image.set(row, col, f_d[image.get(row, col)]);
            }
        }

        return new_image;
    }

    Image equalizeHist(Image image){
        return equalizeHist(image, 0, 255);
    }
}

#endif
