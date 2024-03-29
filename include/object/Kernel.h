#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <complex>

#include "Vector2D.h"
#include "Image.h"
#include "../algorithm/FFT.h"

using namespace std;

namespace dip {
    class Kernel
    {
    private:
        Vector2D<double> kernel;

        vector<vector<complex<double>>> transform_center(int N);
        double log_normalize(double);
    public:
        Kernel();
        Kernel(int, int);
        Kernel(int, int, vector<vector<double>>);
        ~Kernel();
        int c_x = 1, c_y = 1;

        // function
        int rows() const;
        int cols() const;

        void set(int, int, double);
        double get(int, int);

        vector<vector<complex<double>>> transform(int N);
        Image getSpectrumImg(int N);
        Image getPhaseImg(int N);
    };

    
    Kernel::Kernel()
    {
    }

    Kernel::Kernel(int numrows, int numcols)
    {
        this->kernel = Vector2D<double>(numrows, numcols);
        this->c_x = floor(numrows/2.0);
        this->c_y = floor(numcols/2.0);
    }

    Kernel::Kernel(int c_x, int c_y, vector<vector<double>> kernel)
    {
        this->kernel = Vector2D<double>(kernel.size(), kernel[0].size());
        this->c_x = c_x;
        this->c_y = c_y;

        for(int row = 0; row < this->rows(); row++){
            for(int col = 0; col < this->cols(); col++){
                this->kernel.set(row, col, kernel[row][col]);
            }
        }
    }

    Kernel::~Kernel()
    {
    }

    int Kernel::rows() const{
        return this->kernel.rows();
    }

    int Kernel::cols() const{
        return this->kernel.cols();
    }

    void Kernel::set(int x, int y, double d){
        this->kernel.set(x, y, d);
    }

    double Kernel::get(int x, int y){
        if(!(0 <= x && x < this->rows() && 0 <= y && y < this->cols())) return 0;
        return this->kernel.get(x, y);
    }

    double Kernel::log_normalize(double value){
        if(value <= 0) return value;
        return log10(value);
    }

    vector<vector<complex<double>>> Kernel::transform(int N){
        int numRows = kernel.rows();
        int numCols = kernel.cols();

        vector<vector<complex<double>>> input(N, vector<complex<double>>(N));
        
        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                if(row < numRows && col < numCols){
                    input[row][col] = (double)kernel.get(row, col);
                }
            }
        }

        return dip::signal::fft2D(input);
    }

    vector<vector<complex<double>>> Kernel::transform_center(int N){
        int numRows = kernel.rows();
        int numCols = kernel.cols();

        vector<vector<complex<double>>> input(N, vector<complex<double>>(N));
        
        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                double j = 2 * M_PI * (((numRows/2.0) * row + (numCols/2.0) * col) / double(N));
                complex<double> center_shift = polar(1.0, j);
                if(row < numRows && col < numCols){
                    input[row][col] = (double)kernel.get(row, col) * center_shift;
                }
            }
        }

        return dip::signal::fft2D(input);
    }

    Image Kernel::getSpectrumImg(int N){
        Image spectrum_img(N, N);
        vector<vector<complex<double>>> freq_domain_center = this->transform(N);

        double maxVal = log_normalize(abs(freq_domain_center[0][0]));
        double minVal = log_normalize(abs(freq_domain_center[0][0]));
        for (const auto& row : freq_domain_center) {
            for (complex<double> freq : row) {
                double amplitude = log_normalize(abs(freq));
                if (amplitude > maxVal) {
                    maxVal = amplitude;
                }
                if (amplitude < minVal) {
                    minVal = amplitude;
                }
            }
        }

        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                int spectrum = static_cast<int>(255 * (log_normalize(abs(freq_domain_center[row][col])) - minVal) / (maxVal-minVal));
                spectrum_img.set(row, col, spectrum);
            }
        }

        return spectrum_img;
    }

    Image Kernel::getPhaseImg(int N){
        Image phase_img(N, N);
        vector<vector<complex<double>>> freq_domain_center = this->transform(N);

        double maxVal = log_normalize(arg(freq_domain_center[0][0]));
        double minVal = log_normalize(arg(freq_domain_center[0][0]));
        for (const auto& row : freq_domain_center) {
            for (complex<double> freq : row) {
                double phase = log_normalize(arg(freq));
                if (phase > maxVal) {
                    maxVal = phase;
                }
                if (phase < minVal) {
                    minVal = phase;
                }
            }
        }

        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                int spectrum = static_cast<int>(255 * (log_normalize(arg(freq_domain_center[row][col])) - minVal) / (maxVal-minVal));
                phase_img.set(row, col, spectrum);
            }
        }

        return phase_img;
    }
}

#endif
