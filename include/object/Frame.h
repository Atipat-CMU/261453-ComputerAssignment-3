#ifndef FRAME_H
#define FRAME_H

#include <vector>

#include "Pixel.h"

using namespace std;

namespace dip {
    class Frame
    {
    private:
        int xs, ys, xl, yl;
    public:
        Frame();
        Frame(int, int, int, int);
        ~Frame();

        Pixel getTop();
        Pixel getBottom();
    };

    Frame::Frame()
    {
    }

    Frame::Frame(int xs, int ys, int xl, int yl)
    {
        this->xs = xs;
        this->ys = ys;
        this->xl = xl;
        this->yl = yl;
    }
    
    Frame::~Frame()
    {
    }

    Pixel Frame::getTop(){
        return Pixel(xs,ys);
    }

    Pixel Frame::getBottom(){
        return Pixel(xl,yl);
    }
    
}

#endif
