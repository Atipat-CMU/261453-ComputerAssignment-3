#include "../../include/dip.h"

#include <iostream>

using namespace std;
using namespace dip;

int main(int argc, char const *argv[])
{
    Image worm_hole_img = imread("../out/1_WormHole_2H_no_reflection.pgm");
    worm_hole_img = medianFilter(worm_hole_img, 9);
    imwrite(worm_hole_img, "../out/2_WormHole_2H_median_filter.pgm");

    return 0;
}
