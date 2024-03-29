#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <vector>

#include "Vector2D.h"

using namespace std;

namespace dip {
    template<typename T>
    class Vector2D
    {
    private:
        std::vector<std::vector<T>> vec2D;
        size_t numrows;
        size_t numcols;
    public:
        Vector2D();
        Vector2D(size_t rows, size_t cols);
        ~Vector2D();

        int rows() const;
        int cols() const;

        void set(size_t, size_t, const T&);
        T get(size_t, size_t) const;
        std::vector<T> get1D();
        Vector2D<T> crop(size_t, size_t, size_t, size_t);
    };

    template<class T>
    Vector2D<T>::Vector2D()
    {
    }

    template<class T>
    Vector2D<T>::Vector2D(size_t rows, size_t cols)
    {
        this->numrows = rows;
        this->numcols = cols;
        vec2D.resize(rows, std::vector<T>(cols));
    }

    template<class T>
    Vector2D<T>::~Vector2D()
    {
    }

    template<class T>
    int Vector2D<T>::rows() const{
        return this->numrows;
    }

    template<class T>
    int Vector2D<T>::cols() const{
        return this->numcols;
    }

    template<class T>
    void Vector2D<T>::set(size_t row, size_t col, const T& value) {
        if (row < numrows && col < numcols) {
            vec2D[row][col] = value;
        } else {
            std::cerr << "Index out of range!" << std::endl;
        }
    }

    template<class T>
    T Vector2D<T>::get(size_t row, size_t col) const{
        if (row < numrows && col < numcols) {
            return vec2D[row][col];
        } else {
            std::cerr << "Index out of range!" << std::endl;
            return T();
        }
    }

    template<class T>
    vector<T> Vector2D<T>::get1D(){
        vector<T> vec1D;
        for (const auto& row : vec2D) {
            for (T element : row) {
                vec1D.push_back(element);
            }
        }
        return vec1D;
    }

    template<class T>
    Vector2D<T> Vector2D<T>::crop(size_t xs, size_t ys, size_t xl, size_t yl) {
        int c_rows = xl-xs+1;
        int c_cols = yl-ys+1;

        Vector2D<T> croppedVector(c_rows, c_cols);

        for(size_t row = 0; row < c_rows; ++row) {
            for(size_t col = 0; col < c_cols; ++col) {
                croppedVector.set(row, col, vec2D[xs+row][ys+col]);
            }
        }
        return croppedVector;
    }
}

#endif
