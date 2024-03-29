#ifndef GUASS_ELIM_H
#define GUASS_ELIM_H

#include <vector>

using namespace std;

namespace linear {
    vector<double> gaussianElimination(const vector<vector<double>>& A, const vector<double>& b) {
        int n = A.size();
        vector<vector<double>> augmented(n, vector<double>(n + 1, 0));

        // Augment the coefficient matrix with the constants vector
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                augmented[i][j] = A[i][j];
            }
            augmented[i][n] = b[i];
        }

        // Perform forward elimination
        for (int k = 0; k < n - 1; ++k) {
            for (int i = k + 1; i < n; ++i) {
                double factor = augmented[i][k] / augmented[k][k];
                for (int j = k; j <= n; ++j) {
                    augmented[i][j] -= factor * augmented[k][j];
                }
            }
        }

        // Perform back substitution
        vector<double> x(n);
        for (int i = n - 1; i >= 0; --i) {
            x[i] = augmented[i][n];
            for (int j = i + 1; j < n; ++j) {
                x[i] -= augmented[i][j] * x[j];
            }
            x[i] /= augmented[i][i];
        }

        return x;
    }
}

#endif
