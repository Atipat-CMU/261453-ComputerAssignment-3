#ifndef TRANFORM_H
#define TRANFORM_H

#include <complex>

#include "../object/Image.h"
#include "../object/Kernel.h"
#include "../algorithm/FFT.h"

namespace dip { namespace transform {
    class Fourier
    {
        private:
            int numRows;
            int numCols;
            int N;
            vector<vector<complex<double>>> freq_domain;

            vector<vector<complex<double>>> transform_center(Image&);
            vector<vector<complex<double>>> transform(Image&);

            Image inverse_origin(vector<vector<complex<double>>>);
            Image inverse_center(vector<vector<complex<double>>>);

            double log_normalize(double);
            Image inverse_N();
        public:
            Fourier();
            Fourier(Image);
            ~Fourier();

            Image getSpectrumImg();
            Image getPhaseImg();

            Image inverse();
            Image inverseWithoutPhase();
            Image inverseWithoutAmplitude();

            void shift(int x, int y);
            void applyFilter(complex<double> (*fun)(int, int, int, int));
            void applyKernel(Kernel kernel);
    };

    Fourier::Fourier()
    {
    }

    Fourier::Fourier(Image image)
    {
        this->freq_domain = transform(image);
    }
    
    Fourier::~Fourier()
    {
    }

    double Fourier::log_normalize(double value){
        if(value <= 0) return value;
        return log10(value);
    }

    vector<vector<complex<double>>> Fourier::transform(Image& image){
        this->numRows = image.rows();
        this->numCols = image.cols();

        int power = 0;
        if(numRows < numCols) power = ceil(log2(numCols));
        else power = ceil(log2(numRows));

        this->N = pow(2, power);

        vector<vector<complex<double>>> input(N, vector<complex<double>>(N));
        
        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                double j = 2 * M_PI * (((numRows/2.0) * row + (numCols/2.0) * col) / double(N));
                complex<double> center_shift = polar(1.0, j);
                if(row < numRows && col < numCols){
                    input[row][col] = (double)image.get(row, col);
                }
            }
        }
        return dip::signal::fft2D(input);
    }

    vector<vector<complex<double>>> Fourier::transform_center(Image& image){
        this->numRows = image.rows();
        this->numCols = image.cols();

        int power = 0;
        if(numRows < numCols) power = ceil(log2(numCols));
        else power = ceil(log2(numRows));

        this->N = pow(2, power);

        vector<vector<complex<double>>> input(N, vector<complex<double>>(N));
        
        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                double j = 2 * M_PI * (((numRows/2.0) * row + (numCols/2.0) * col) / double(N));
                complex<double> center_shift = polar(1.0, j);
                if(row < numRows && col < numCols){
                    input[row][col] = (double)image.get(row, col) * center_shift;
                }
            }
        }

        return dip::signal::fft2D(input);
    }

    Image Fourier::inverse(){
        return inverse_origin(this->freq_domain);
    }

    Image Fourier::inverse_origin(vector<vector<complex<double>>> freq_domain){
        vector<vector<complex<double>>> output = dip::signal::ifft2D(freq_domain);

        Image out_image(numRows, numCols);

        for(int row = 0; row < this->numRows; row++){
            for(int col = 0; col < this->numCols; col++){
                out_image.set(row, col, (int)((output[row][col]).real()));
            }
        }

        return out_image;
    }

    Image Fourier::inverse_center(vector<vector<complex<double>>> freq_domain){
        vector<vector<complex<double>>> output = dip::signal::ifft2D(freq_domain);

        Image out_image(numRows, numCols);

        for(int row = 0; row < this->numRows; row++){
            for(int col = 0; col < this->numCols; col++){
                double j = 2 * M_PI * (((numRows/2.0) * row + (numCols/2.0) * col) / double(N));
                complex<double> center_shift = polar(1.0, j);
                out_image.set(row, col, (int)((output[row][col] / center_shift).real()));
            }
        }

        return out_image;
    }

    Image Fourier::inverse_N(){
        vector<vector<complex<double>>> output = dip::signal::ifft2D(freq_domain);

        Image out_image(N, N);

        for(int row = 0; row < this->N; row++){
            for(int col = 0; col < this->N; col++){
                out_image.set(row, col, (int)((output[row][col]).real()));
            }
        }

        return out_image;
    }

    Image Fourier::inverseWithoutPhase(){
        vector<vector<complex<double>>> freq_domain_without_phase(N, vector<complex<double>>(N));
        for(int i = 0; i < this->N; i++){
            for(int j = 0; j < this->N; j++){
                freq_domain_without_phase[i][j] = abs(freq_domain[i][j]);
            }
        }

        vector<vector<complex<double>>> output = dip::signal::ifft2D(freq_domain_without_phase);

        Image out_image(N, N);

        for(int row = 0; row < this->N; row++){
            for(int col = 0; col < this->N; col++){
                out_image.set(row, col, (int)((output[row][col]).real()));
            }
        }

        return out_image;
    }

    Image Fourier::inverseWithoutAmplitude(){
        vector<vector<complex<double>>> freq_domain_without_amp(N, vector<complex<double>>(N));

        // Remove amplitude information
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                complex<double>& freq = freq_domain[i][j];
                double magnitude = abs(freq);
                freq_domain_without_amp[i][j] = (magnitude == 0) ? complex<double>(0, 0) : freq / magnitude;
            }
        }

        vector<vector<complex<double>>> output = dip::signal::ifft2D(freq_domain_without_amp);

        double maxVal = log_normalize(arg(output[0][0]));
        double minVal = log_normalize(arg(output[0][0]));
        for (const auto& row : output) {
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

        Image out_image(numRows, numCols);

        for(int row = 0; row < this->numRows; row++){
            for(int col = 0; col < this->numCols; col++){
                out_image.set(row, col, (int)(255 * (log_normalize(abs(output[row][col])) - minVal) / (maxVal-minVal)));
            }
        }

        return out_image;
    }

    Image Fourier::getSpectrumImg(){
        Image image = inverse();
        vector<vector<complex<double>>> freq_domain_center = transform_center(image);

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

        Image spectrum_img(numRows, numCols);

        for(int row = 0; row < this->numRows; row++){
            for(int col = 0; col < this->numCols; col++){
                int spectrum = static_cast<int>(255 * (log_normalize(abs(freq_domain_center[row][col])) - minVal) / (maxVal-minVal));
                spectrum_img.set(row, col, spectrum);
            }
        }

        return spectrum_img;
    }

    Image Fourier::getPhaseImg(){
        Image image = inverse();
        vector<vector<complex<double>>> freq_domain_center = transform_center(image);

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

        Image phase_img(numRows, numCols);

        for(int row = 0; row < this->numRows; row++){
            for(int col = 0; col < this->numCols; col++){
                int spectrum = static_cast<int>(255 * (log_normalize(arg(freq_domain_center[row][col])) - minVal) / (maxVal-minVal));
                phase_img.set(row, col, spectrum);
            }
        }

        return phase_img;
    }

    void Fourier::shift(int x, int y){
        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                double j = -2 * M_PI * ((x*row + y*col) / double(N));
                complex<double> phase_shift = polar(1.0, j);
                this->freq_domain[row][col] = phase_shift * freq_domain[row][col];
            }
        }
    }

    void Fourier::applyFilter(complex<double> (*fun)(int u, int v, int M, int N)){
        Image input = inverse();
        vector<vector<complex<double>>> freq_domain_center = transform_center(input);

        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                freq_domain_center[row][col] = fun(row, col, numRows, numCols) * freq_domain_center[row][col];
            }
        }

        Image output = inverse_center(freq_domain_center);
        this->freq_domain = transform(output);
    }

    void Fourier::applyKernel(Kernel kernel){
        Image input = inverse();
        input = input.pad(1,1,1,1);
        this->freq_domain = transform(input);

        vector<vector<complex<double>>> kernel_freq_domain = kernel.transform(N);
        for(int row = 0; row < N; row++){
            for(int col = 0; col < N; col++){
                this->freq_domain[row][col] = kernel_freq_domain[row][col] * this->freq_domain[row][col];
            }
        }

        Image output = inverse_N();
        output = output.unpad(2,2,N-numRows,N-numCols);
        this->freq_domain = transform(output);
    }
}}

#endif
