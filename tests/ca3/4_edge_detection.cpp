#include "../../include/dip.h"

#include <iostream>

using namespace std;
using namespace dip;

int main(int argc, char const *argv[])
{
    Image worm_hole_img = imread("../out/3_WormHole_2H_gaussian_blur.pgm");

    Edge img_edge(worm_hole_img);

    Image grad_x_img = img_edge.imgGradX();
    imwrite(grad_x_img, "../out/4/4_WormHole_2H_g_x.pgm");

    Image grad_y_img = img_edge.imgGradY();
    imwrite(grad_y_img, "../out/4/4_WormHole_2H_g_y.pgm");

    Image magnitude_img = img_edge.imgMagitude();
    imwrite(magnitude_img, "../out/4/4_WormHole_2H_edge_magnitude.pgm");

    Image thin_edge_img = img_edge.imgThinEdge();
    imwrite(thin_edge_img, "../out/4/4_WormHole_2H_thin_edge.pgm");

    Image dt_edge_img = img_edge.imgDTEdge();
    imwrite(dt_edge_img, "../out/4/4_WormHole_2H_doubleT_edge.pgm");

    Image edge_img = img_edge.imgEdge();
    imwrite(edge_img, "../out/4/4_WormHole_2H_edge.pgm");

    return 0;
}
