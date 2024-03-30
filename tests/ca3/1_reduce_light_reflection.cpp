#include "../../include/dip.h"

#include <iostream>
#include <vector>
#include <random>

using namespace std;
using namespace dip;

int eq_reduce_light(int x, int Dmax){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(-25, 25);
    int rand = dis(gen);
    if(x >= Dmax-(Dmax*0.2)) return (x/1.5)+rand;
    return x;
}

int main(int argc, char const *argv[])
{
    Image worm_hole_img = imreadTIF("../src/WormHole_2H.tif");
    imwrite(worm_hole_img, "../out/WormHole_2H.pgm");
    worm_hole_img = poperation(worm_hole_img, eq_reduce_light);
    imwrite(worm_hole_img, "../out/1_WormHole_2H_no_reflection.pgm");

    return 0;
}
