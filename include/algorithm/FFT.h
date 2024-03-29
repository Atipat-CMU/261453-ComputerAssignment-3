#ifndef FFT_H
#define FFT_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <complex>
#include <vector>
#include <cmath>

using namespace std;

namespace dip { namespace signal {
    // Function to perform 1D FFT along the rows of a 2D matrix
    void fft1D(vector<complex<double>>& row) {
        int N = row.size();
        if (N <= 1) {
            return;
        }

        // Split even and odd elements
        vector<complex<double>> even(N/2), odd(N/2);
        for (int i = 0; i < N/2; ++i) {
            even[i] = row[i * 2];
            odd[i] = row[i * 2 + 1];
        }

        // Recursively compute FFT on even and odd halves
        fft1D(even);
        fft1D(odd);

        // Combine results of even and odd halves
        for (int k = 0; k < N/2; ++k) {
            complex<double> t = polar(1.0, -2.0 * M_PI * k / N) * odd[k];
            row[k] = even[k] + t;
            row[k + N/2] = even[k] - t;
        }
    }

    vector<vector<complex<double>>> fft2D(const vector<vector<complex<double>>>& input) {
        int numRows = input.size();
        int numCols = input[0].size();

        vector<vector<complex<double>>> output(numRows, vector<complex<double>>(numCols));

        // Perform 1D FFT along the rows
        for (int i = 0; i < numRows; ++i) {
            vector<complex<double>> row(numCols);
            for (int j = 0; j < numCols; ++j) {
                row[j] = input[i][j];
            }
            fft1D(row); // Assuming fft1D is defined elsewhere to perform 1D FFT
            output[i] = row;
        }

        // Transpose the result
        vector<vector<complex<double>>> transposed(numCols, vector<complex<double>>(numRows));
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                transposed[j][i] = output[i][j];
            }
        }

        // Perform 1D FFT along the columns
        for (int i = 0; i < numCols; ++i) {
            vector<complex<double>>& col = transposed[i];
            fft1D(col); // Assuming fft1D is defined elsewhere to perform 1D FFT
        }

        // Transpose back the result
        for (int i = 0; i < numCols; ++i) {
            for (int j = 0; j < numRows; ++j) {
                output[j][i] = transposed[i][j];
            }
        }

        return output;
    }

    // Function to perform 1D inverse FFT
    void ifft1D(vector<complex<double>>& spectrum) {
        int N = spectrum.size();
        if (N <= 1) {
            return;
        }

        // Conjugate the spectrum
        for (auto& val : spectrum) {
            val = conj(val);
        }

        // Perform forward FFT
        fft1D(spectrum);

        // Scale the result
        for (auto& val : spectrum) {
            val /= N;
        }

        // Conjugate the result again
        for (auto& val : spectrum) {
            val = conj(val);
        }
    }

    // Function to perform 2D inverse FFT
    vector<vector<complex<double>>> ifft2D(const vector<vector<complex<double>>>& spectrum) {
        int numRows = spectrum.size();
        int numCols = spectrum[0].size();

        // Perform IFFT along the rows
        vector<vector<complex<double>>> transposed(numCols, vector<complex<double>>(numRows));
        for (int i = 0; i < numRows; ++i) {
            vector<complex<double>> row(numCols);
            for (int j = 0; j < numCols; ++j) {
                row[j] = spectrum[i][j];
            }
            ifft1D(row);
            for (int j = 0; j < numCols; ++j) {
                transposed[j][i] = row[j];
            }
        }

        // Perform IFFT along the columns
        for (int i = 0; i < numCols; ++i) {
            ifft1D(transposed[i]);
        }

        // Transpose the result back
        vector<vector<complex<double>>> output(numCols, vector<complex<double>>(numRows));
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                output[j][i] = transposed[i][j];
            }
        }

        return output;
    }
}}

#endif
