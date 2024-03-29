#ifndef EDGE_H
#define EDGE_H

#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <iostream>

#include "Image.h"
#include "Kernel.h"
#include "Vector2D.h"

using namespace std;

namespace dip  {
    class Edge
    {
    private:
        Image image;
        Vector2D<double> magnitude;
        Vector2D<double> phase;
        Vector2D<double> gradX;
        Vector2D<double> gradY;
        Vector2D<double> thin_edge;
        Vector2D<double> doubleT_edge;
        Vector2D<double> edge;

        double log_normalize(double);
        Image img_normalize(Vector2D<double>);

        void computeSobel();
        void computeCanny();
        Vector2D<double> computeGradient(Kernel);
        Vector2D<double> computeMagitude(Vector2D<double> gradX, Vector2D<double> gradY);
        Vector2D<double> computePhase(Vector2D<double> gradX, Vector2D<double> gradY);
        Vector2D<double> computeNonMaxSup(Vector2D<double> magnitude, Vector2D<double> phase);
        Vector2D<double> threshold(Vector2D<double> edge, double highT_rate, double lowT_rate);
        Vector2D<double> edgePlant(Vector2D<double> edge_dt);
        void edgeGrowing(const Vector2D<double>& edge_dt, Vector2D<double>& edge, int row, int col);
    public:
        Edge();
        Edge(Image);
        ~Edge();

        Image imgGradX();
        Image imgGradY();
        Image imgMagitude();
        Image imgThinEdge();
        Image imgDTEdge();
        Image imgEdge();
    };

    Edge::Edge()
    {
    }
    
    Edge::Edge(Image img)
    {
        this->image = img;
        computeCanny();
    }
    
    Edge::~Edge()
    {
    }

    double Edge::log_normalize(double value){
        if(value <= 0) return value;
        return log10(value);
    }

    Image Edge::img_normalize(Vector2D<double> values){
        int numRows = values.rows();
        int numCols = values.cols();

        double maxVal = values.get(0,0);
        double minVal = values.get(0,0);
        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                double norm_value = values.get(row, col);
                if (norm_value > maxVal) {
                    maxVal = norm_value;
                }
                if (norm_value < minVal) {
                    minVal = norm_value;
                }
            }
        }

        Image output(numRows, numCols);

        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                int value = static_cast<int>(255 * (values.get(row, col) - minVal) / (maxVal-minVal));
                output.set(row, col, value);
            }
        }
        return output;
    }

    void Edge::computeSobel(){
        Kernel kernel_gradX(1, 1,
        {
            {-1,  0,  1},
            {-2,  0,  2},
            {-1,  0,  1}
        });
        this->gradX = computeGradient(kernel_gradX);

        Kernel kernel_gradY(1, 1,
        {
            {-1, -2, -1},
            { 0,  0,  0},
            { 1,  2,  1}
        });
        this->gradY = computeGradient(kernel_gradY);
        this->magnitude = computeMagitude(this->gradX, this->gradY);
        this->phase = computePhase(this->gradX, this->gradY);
    }

    void Edge::computeCanny(){
        computeSobel();
        this->thin_edge = computeNonMaxSup(this->magnitude, this->phase);
        this->doubleT_edge = threshold(this->thin_edge, 0.6, 0.15);
        this->edge = edgePlant(this->doubleT_edge);
    }

    Vector2D<double> Edge::computeGradient(Kernel kernel){
        Vector2D<double> output(this->image.rows(), this->image.cols());

        for(int row = 3; row < this->image.rows()-3; row++){
            for(int col = 3; col < this->image.cols()-3; col++){
                double sum = 0;
                for(int i = 0; i < kernel.rows(); i++){
                    for(int k = 0; k < kernel.cols(); k++){
                        sum += this->image.getOutZero(row-kernel.c_x+i, col-kernel.c_y+k) * kernel.get(i,k);
                    }
                }
                output.set(row, col, sum);
            }
        }

        return output;
    }

    Vector2D<double> Edge::computeMagitude(Vector2D<double> gradX, Vector2D<double> gradY){
        Vector2D<double> output(this->image.rows(), this->image.cols());

        for(int row = 0; row < this->image.rows(); row++){
            for(int col = 0; col < this->image.cols(); col++){
                output.set(row, col, sqrt(pow(gradX.get(row, col),2) + pow(gradY.get(row, col),2)));
            }
        }
        return output;
    }

    Vector2D<double> Edge::computePhase(Vector2D<double> gradX, Vector2D<double> gradY){
        Vector2D<double> output(this->image.rows(), this->image.cols());

        for(int row = 0; row < this->image.rows(); row++){
            for(int col = 0; col < this->image.cols(); col++){
                output.set(row, col, (atan(gradX.get(row, col)/gradY.get(row, col))) * 180 / M_PI);
            }
        }
        return output;
    }

    Vector2D<double> Edge::computeNonMaxSup(Vector2D<double> magnitude, Vector2D<double> phase) {
        Vector2D<double> output(this->image.rows(), this->image.cols());

        for(int row = 1; row < this->image.rows() - 1; row++) {
            for(int col = 1; col < this->image.cols() - 1; col++) {
                double my_magnitude = magnitude.get(row, col);

                // find neighboring pixels with gradient angle
                double neighbor_1, neighbor_2;
                double angle = phase.get(row, col);
                if (angle < 0) angle += 180;

                if ((0 <= angle && angle < 22.5) || (157.5 <= angle && angle <= 180)) {
                    neighbor_1 = magnitude.get(row + 1, col);
                    neighbor_2 = magnitude.get(row - 1, col);
                } else if (22.5 <= angle && angle < 67.5) {
                    neighbor_1 = magnitude.get(row - 1, col - 1);
                    neighbor_2 = magnitude.get(row + 1, col + 1);
                } else if (67.5 <= angle && angle < 112.5) {
                    neighbor_1 = magnitude.get(row, col - 1);
                    neighbor_2 = magnitude.get(row, col + 1);
                } else if (112.5 <= angle && angle < 157.5) {
                    neighbor_1 = magnitude.get(row + 1, col - 1);
                    neighbor_2 = magnitude.get(row - 1, col + 1);
                }

                // non-maximum suppression
                if (my_magnitude < neighbor_1 || my_magnitude < neighbor_2) {
                    output.set(row, col, 0);
                } else {
                    output.set(row, col, my_magnitude);
                }
            }
        }

        return output;
    }

    Vector2D<double> Edge::threshold(Vector2D<double> edge, double highT_rate, double lowT_rate){
        Vector2D<double> output(this->image.rows(), this->image.cols());

        double max = 0;

        for(int row = 0; row < this->image.rows(); row++){
            for(int col = 0; col < this->image.cols(); col++){
                if(edge.get(row, col) > max) max = edge.get(row, col);
            }
        }

        double highT = max*highT_rate;
        double lowT = max*lowT_rate;

        for(int row = 0; row < this->image.rows(); row++){
            for(int col = 0; col < this->image.cols(); col++){
                if(edge.get(row, col) >= highT){
                    output.set(row, col, 255);
                }else if(edge.get(row, col) >= lowT){
                    output.set(row, col, 25);
                }
            }
        }
        return output;
    }

    void Edge::edgeGrowing(const Vector2D<double>& edge_dt, Vector2D<double>& edge, int row, int col) {
        stack<Pixel> stack;
        stack.push(Pixel(row, col));

        while (!stack.empty()) {
            Pixel current = stack.top();
            stack.pop();

            edge.set(current.x(), current.y(), 255);

            for(int i = -1; i <= 1; i++){
                for(int j = -1; j <= 1; j++){
                    Pixel next_pixel(current.x() + i, current.y() + j);
                    if(edge.get(next_pixel.x(), next_pixel.y()) != 255) {
                        if(edge_dt.get(next_pixel.x(), next_pixel.y()) == 25) {
                            stack.push(next_pixel);
                        }
                    }
                }
            }
        }
    }

    Vector2D<double> Edge::edgePlant(Vector2D<double> edge_dt) {
        Vector2D<double> edge(this->image.rows(), this->image.cols());\

        for(int row = 0; row < this->image.rows(); row++){
            for(int col = 0; col < this->image.cols(); col++){
                if(edge_dt.get(row, col) == 255) edgeGrowing(edge_dt, edge, row, col);
            }
        }
        return edge;
    }

    Image Edge::imgGradX(){
        return img_normalize(this->gradX);
    }

    Image Edge::imgGradY(){
        return img_normalize(this->gradY);
    }

    Image Edge::imgMagitude(){
        return img_normalize(this->magnitude);
    }

    Image Edge::imgThinEdge(){
        return img_normalize(this->thin_edge);
    }

    Image Edge::imgDTEdge(){
        return img_normalize(this->doubleT_edge);
    }

    Image Edge::imgEdge(){
        return img_normalize(this->edge);
    }
}

#endif
