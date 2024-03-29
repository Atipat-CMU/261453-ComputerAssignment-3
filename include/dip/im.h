#ifndef IM_H
#define IM_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <tiffio.h>

#include "../object/Image.h"

using namespace std;

namespace dip {

    Image imreadTIF(string filename) {
        TIFF* tif = TIFFOpen(filename.c_str(), "r");
        if (!tif) {
            throw std::runtime_error("Error opening TIFF file: " + filename);
        }

        // Get image dimensions
        uint32_t width, height;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

        // Read image data
        Vector2D<int> image(height, width); // Note: Swapped height and width to match your Vector2D convention
        uint32_t* raster = (uint32_t*)_TIFFmalloc(width * height * sizeof(uint32_t));
        if (!raster) {
            TIFFClose(tif);
            throw std::runtime_error("Memory allocation failed");
        }

        if (TIFFReadRGBAImage(tif, width, height, raster, 0)) {
            for (uint32_t row = 0; row < height; ++row) {
                for (uint32_t col = 0; col < width; ++col) {
                    uint32_t index = row * width + col;
                    uint32_t pixel = raster[index];
                    uint8_t r = TIFFGetR(pixel);
                    uint8_t g = TIFFGetG(pixel);
                    uint8_t b = TIFFGetB(pixel);
                    uint8_t grayscale = static_cast<uint8_t>(0.21 * r + 0.72 * g + 0.07 * b);
                    image.set(row, col, grayscale);
                }
            }
        }

        // Clean up
        _TIFFfree(raster);
        TIFFClose(tif);

        return Image(image);
    }
    
    Image imread(string filename){
        int count = 0;

        string version;
        int numrows = 0, numcols = 0;
        int max_val;

        ifstream file(filename, ios::binary);
        stringstream ss;
        stringstream temp;    
        string line;

        if (!file.is_open()) {
            throw runtime_error("File not found: " + filename);
        }

        while (getline(file, line))
        {
            if(line[0] != '#'){
                if(count == 0){ version = line; }
                if(count == 1){ temp << line; temp >> numcols >> numrows; }
                if(count == 2){ temp.clear(); temp << line; temp >> max_val; }
                count++;
            }
            if(count == 3) break;
        }
        
        int imsize = 0;

        Vector2D<int> image = Vector2D<int>(numrows, numcols);

        char pixel;

        for(int row = 0; row < numrows; row++){
            for(int col = 0; col < numcols; col++){
                file.read(&pixel, 1);
                image.set(row, col, static_cast<int>(static_cast<unsigned char>(pixel)));
            }
        }
        file.close();

        return Image(image);
    }

    void imwrite(Image& image, string filename) {
        ofstream file(filename, ios::binary);
        file << "P5" << endl;
        file << image.cols() << " " << image.rows() << endl;
        file << "255" << endl;

        for (int row = 0; row < image.rows(); row++) {
            for (int col = 0; col < image.cols(); col++) {
                int d = image.get(row, col);
                if (d < 0) d = 0;
                if (d > 255) d = 255;
                unsigned char pixel = static_cast<unsigned char>(d);
                file.write(reinterpret_cast<char*>(&pixel), sizeof(unsigned char));
            }
        }

        file.close();
    }

    void imshow(Image& image){
        for(int row = 0; row < image.rows(); ++row) {
            for(int col = 0; col < image.cols(); ++col) {
                cout << image.get(row, col) << " ";
            }
            cout << endl;
        }
    }
}

#endif
