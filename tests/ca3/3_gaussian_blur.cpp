#include "../../include/dip.h"

#include <iostream>

using namespace std;
using namespace dip;

int main(int argc, char const *argv[])
{
    Image worm_hole_img = imread("../out/2_WormHole_2H_median_filter.pgm");
    Kernel gaussian = GaussianKernel(3, 3);
    worm_hole_img = convolution(worm_hole_img, gaussian);
    imwrite(worm_hole_img, "../out/3_WormHole_2H_gaussian_blur.pgm");

    return 0;
}
