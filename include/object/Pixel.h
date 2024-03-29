#ifndef PIXEL_H
#define PIXEL_H

namespace dip {

    class Pixel
    {
    private:
        int x_ = 0, y_ = 0;
    public:
        int x();
        int y();
        Pixel();
        Pixel(int, int);
        ~Pixel();
    };

    Pixel::Pixel()
    {
    }

    int Pixel::x(){
        return x_;
    }

    int Pixel::y(){
        return y_;
    }
    
    Pixel::Pixel(int x, int y)
    {
        this->x_ = x;
        this->y_ = y;
    }
    
    Pixel::~Pixel()
    {
    }
}

#endif
