#ifndef MASK_H
#define MASK_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "Vector2D.h"
#include "Pixel.h"

using namespace std;

namespace dip  {
    class Mask
    {
    private:
        Vector2D<int> mask;

        bool isOutOfMask(Pixel);
    public:
        Mask();
        Mask(int, int);
        ~Mask();

        int rows() const;
        int cols() const;

        void set(Pixel, int);
        void set(int, int, int);

        int get(Pixel);
        int get(int, int);

        Mask toBinary(int);
        void toTextFile(string, int);
        void fromTextFile(string, int);
    };
    
    Mask::Mask()
    {
    }

    Mask::Mask(int numrows, int numcols)
    {
        this->mask = Vector2D<int>(numrows, numcols);
    }
    
    Mask::~Mask()
    {
    }

    int Mask::rows() const{
        return this->mask.rows();
    }

    int Mask::cols() const{
        return this->mask.cols();
    }

    void Mask::set(Pixel p, int d){
        if(isOutOfMask(p)) return;
        this->mask.set(p.x(), p.y(), d);
    }

    void Mask::set(int x, int y, int d){
        this->set(Pixel(x, y), d);
    }

    int Mask::get(Pixel p) {
        if(isOutOfMask(p)) return -1;
        return this->mask.get(p.x(), p.y());
    }

    int Mask::get(int x, int y){
        return this->get(Pixel(x, y));
    }

    bool Mask::isOutOfMask(Pixel p){
        int x = p.x();
        int y = p.y();
        return !((0 <= x && x < this->rows()) && (0 <= y && y < this->cols()));
    }

    Mask Mask::toBinary(int value){
        Mask binary(this->rows(), this->cols());

        for(size_t row = 0; row < this->rows(); ++row) {
            for(size_t col = 0; col < this->cols(); ++col) {
                if(mask.get(row, col) == value) {
                    binary.set(row, col, 1);
                }
                else binary.set(row, col, 0);
            }
        }

        return binary;
    }

    void Mask::toTextFile(string filename, int value){
        ofstream myfile(filename);

        if (myfile.is_open())
        {
            for(size_t row = 0; row < this->rows(); ++row) {
                for(size_t col = 0; col < this->cols(); ++col) {
                    if(mask.get(row, col) == value) {
                        myfile << row << " " << col << "\n";
                    }
                }
            }
            myfile.close();
        }
    }

    void Mask::fromTextFile(string filename, int value){
        string line;
        ifstream myfile(filename);

        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                istringstream iss(line);
                string x_str, y_str;

                if (getline(iss, x_str, ' ') && getline(iss, y_str, ' ')) {
                    int x = stoi(x_str);
                    int y = stoi(y_str);
                    this->set(x,y,value);
                }
            }
            myfile.close();
        }
    }
}

#endif
