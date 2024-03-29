#ifndef MOMENT_H
#define MOMENT_H

#include <vector>
#include <iostream>
#include <cmath>

#include "Image.h"
#include "Mask.h"

using namespace std;

namespace dip {
    class Moment
    {
    private:
        Mask binaryMask;
        Image image;
        bool isBinary = true;
        int m[2][2];

        int moment(int, int, int, int);
        int moment_binary(int, int, int, int);
    public:
        Moment();
        Moment(Mask);
        Moment(Mask,Image);
        ~Moment();
        int getCentralMoment(int, int);
        float getInvariant();
    };

    Moment::Moment()
    {
    }

    Moment::Moment(Mask binaryMask)
    {
        this->binaryMask = binaryMask;
        
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                m[i][j] = moment_binary(i, j, 0, 0);
            }
        }
    }

    Moment::Moment(Mask binaryMask, Image image)
    {
        this->binaryMask = binaryMask;
        this->image = image;
        this->isBinary = false;
        
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                m[i][j] = moment(i, j, 0, 0);
            }
        }
    }
    
    Moment::~Moment()
    {
    }

    int Moment::moment_binary(int p, int q, int _x, int _y){
        int result = 0;

        for(int x = 0; x < binaryMask.rows(); x++){
            for(int y = 0; y < binaryMask.cols(); y++){
                result += (pow(x-_x, p) * pow(y-_y, q)) * binaryMask.get(x, y);
            }
        }

        return result;
    }

    int Moment::moment(int p, int q, int _x, int _y){
        int result = 0;

        for(int x = 0; x < binaryMask.rows(); x++){
            for(int y = 0; y < binaryMask.cols(); y++){
                if(image.get(x, y) == 200)
                result += (pow(x-_x, p) * pow(y-_y, q));
            }
        }

        return result;
    }

    int Moment::getCentralMoment(int p, int q){
        int _x = m[1][0]/m[0][0];
        int _y = m[0][1]/m[0][0];
        if(isBinary) return moment_binary(p, q, _x, _y);
        else return moment(p, q, _x, _y);
    }

    float Moment::getInvariant(){
        float n02 = this->getCentralMoment(0,2)/pow(this->getCentralMoment(0,0), 2);
        float n20 = this->getCentralMoment(2,0)/pow(this->getCentralMoment(0,0), 2);
        return n02 + n20;
    }
}

#endif
