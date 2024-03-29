#ifndef BLOBCOLORING_H
#define BLOBCOLORING_H

#include <vector>
#include <map>
#include <iostream>
using namespace std;

#include "../object/Image.h"
#include "../object/Mask.h"
#include "../object/Frame.h"

namespace dip {
    class BlobColoring
    {
    private:
        int num;
        Image image;
        int min, max;
        Mask labeling;
        vector<Frame> frames;
        bool isComponent(int);

        vector<Frame> coloring();
    public:
        BlobColoring();
        BlobColoring(Image, int, int);
        ~BlobColoring();
        Mask getLabelMask();
        vector<Frame> getComponents();
        int nums();
    };

    BlobColoring::BlobColoring()
    {
    }
    
    BlobColoring::BlobColoring(Image image, int min, int max)
    {
        this->image = image;
        this->min = min;
        this->max = max;
        this->frames = this->coloring();
    }
    
    BlobColoring::~BlobColoring()
    {
    }

    bool BlobColoring::isComponent(int d){
        if(d == -1) return false;
        return min <= d && d <= max;
    }

    vector<Frame> BlobColoring::coloring(){
        int k = 1;
        map<int,int> eq_table;
        map<int,int> label_map;
        vector<vector<int>> blob_table;

        this->labeling = Mask(image.rows(), image.cols());

        for(int x = 0; x < image.rows(); x++){
            for(int y = 0; y < image.cols(); y++){
                Pixel p = Pixel(x,y);
                if(isComponent(image.get(p))){
                    int count = 0;
                    int min = k;
                    int value[5];
                    for(int i = -1; i <= 1; i++){
                        for(int j = -1; j <= 1; j++){
                            if(count > 4) break;
                            Pixel np = Pixel(p.x()+i,p.y()+j);
                            if(labeling.get(np) > 0 && labeling.get(np) < min){
                                min = labeling.get(np);
                            }
                            value[count] = labeling.get(np);
                            count++;
                        }
                        if(count > 4) break;
                    }
                    if(min == k) {
                        labeling.set(p, k++);
                    }
                    else {
                        labeling.set(p, min);
                        for(int i = 0; i < 4; i++){
                            if(value[i] >= min){
                                eq_table[value[i]] = min;
                            }
                        }
                    }
                }
            }
        }

        for(const auto& pair : eq_table){
            if(pair.first == pair.second){
                vector<int> label;
                label.push_back(pair.second);
                blob_table.push_back(label);
            }else{
                for (int blob = 0; blob < blob_table.size(); blob++) {
                    for (size_t i = 0; i < blob_table[blob].size(); ++i) {
                        if (blob_table[blob][i] == pair.second) {
                            blob_table[blob].push_back(pair.first);
                            break;
                        }
                    }
                }
            }
        }

        for (int blob = 0; blob < blob_table.size(); blob++) {
            for (size_t i = 0; i < blob_table[blob].size(); ++i) {
                label_map[blob_table[blob][i]] = blob + 1;
            }
        }

        for(int x = 0; x < labeling.rows(); x++){
            for(int y = 0; y < labeling.cols(); y++){
                Pixel p = Pixel(x,y);
                labeling.set(p, label_map[labeling.get(p)]);

            }
        }

        this->num = blob_table.size();

        vector<Frame> frames;

        for (int blob = 1; blob <= blob_table.size(); blob++) {
            int xs = labeling.rows(), ys = labeling.cols(), xl = 0, yl = 0;
            for(int x = 0; x < labeling.rows(); x++){
                for(int y = 0; y < labeling.cols(); y++){
                    Pixel p = Pixel(x,y);
                    if(labeling.get(p) == blob){
                        if(x < xs) xs = x;
                        if(y < ys) ys = y;
                        if(x > xl) xl = x;
                        if(y > yl) yl = y;
                    }
                }
            }
            frames.emplace_back(Frame(xs, ys, xl, yl));
        }

        return frames;
    }

    Mask BlobColoring::getLabelMask(){
        return this->labeling;
    }

    vector<Frame> BlobColoring::getComponents(){
        return this->frames;
    }

    int BlobColoring::nums(){
        return this->num;
    }
}

#endif
