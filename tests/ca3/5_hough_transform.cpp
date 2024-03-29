#include "../../include/dip.h"

#include <iostream>

using namespace std;
using namespace dip;

int main(int argc, char const *argv[])
{
    Image worm_hole_img = imreadTIF("../src/WormHole_2H.tif");
    Image worm_hole_egde_img = imread("../out/4/4_WormHole_2H_edge.pgm");
    int numRows = worm_hole_egde_img.rows();
    int numCols = worm_hole_egde_img.cols();

    Image worm_hole_blur_img = imread("../out/3_WormHole_2H_gaussian_blur.pgm");
    Image hole_segment(numRows, numCols);

    int min_r = 5;
    int max_r = 12;

    for(int r = min_r; r <= max_r; r++){
        Image hough(numRows, numCols);
        int max = 0;
        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                if(worm_hole_egde_img.get(row, col) == 255){
                    for(int deg = 0; deg < 360; deg++){
                        int x = row - r * cos(deg * M_PI / 180);
                        int y = col - r * sin(deg * M_PI / 180);
                        if(x >= 0 && y >= 0 && x < numRows && y < numCols){
                            hough.set(x, y, hough.get(x, y) + 1);
                        }
                    }
                    if(hough.get(row, col) > max) max = hough.get(row, col);
                }
            }
        }

        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                if(hough.get(row, col) < max*0.99){
                    hough.set(row, col, 0);
                }else{
                    hough.set(row, col, 255);
                }
            }
        }

        Kernel kernel(1, 1,
        {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        });

        hough = erosion(hough, kernel);

        int threshold_hole_max = 50;
        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                if(hough.get(row, col) == 255 && worm_hole_blur_img.get(row, col) < threshold_hole_max){
                    stack<Pixel> stack;
                    stack.push(Pixel(row, col));

                    while (!stack.empty()) {
                        Pixel current = stack.top();
                        stack.pop();

                        hole_segment.set(current.x(), current.y(), 255);

                        for(int i = -1; i <= 1; i++){
                            for(int j = -1; j <= 1; j++){
                                Pixel next_pixel(current.x() + i, current.y() + j);
                                if(hole_segment.get(next_pixel.x(), next_pixel.y()) != 255) {
                                    if(worm_hole_blur_img.get(next_pixel.x(), next_pixel.y()) < threshold_hole_max) {
                                        stack.push(next_pixel);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    BlobColoring blobs(hole_segment, 250, 255);
    cout << "Number of worm holes: " <<blobs.nums() << endl;

    Mask component_mask = blobs.getLabelMask();
    Mask center_of_component(numRows, numCols);

    for(int k = 1; k <= blobs.nums(); k++){
        int t = INT_MAX, l = INT_MAX, b = 0, r = 0;
        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                if(component_mask.get(row, col) == k){
                    if(row < t) t = row;
                    if(col < l) l = col;
                    if(row > b) b = row;
                    if(col > r) r = col;
                }
            }
        }
        int x_center = t+((b-t)/2);
        int y_center = l+((r-l)/2);
        center_of_component.set(x_center, y_center, 1);
        cout << "[" << k << "] x: " << x_center << "  y: " << y_center << endl;
    }

    Image output = worm_hole_img.highlight(center_of_component, 5);
    imwrite(output, "../out/_final_WormHole_2H.pgm");

    return 0;
}
