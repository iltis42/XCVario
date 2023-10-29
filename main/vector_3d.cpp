#include "vector_3d.h"
#include <cstdint>
#include <limits>
#include <cassert>

template <typename T>
vector_3d<T>::vector_3d(T _a, T _b, T _c)
{
    a = _a;
    b = _b;
    c = _c;
}

template <typename T>
vector_3d<T>& vector_3d<T>::operator+=(const vector_3d<T>& v2)
{
    a = a + v2.a;
    b = b + v2.b;
    c = c + v2.c;
    return *this;
}

template <typename T>
vector_3d<T>&  vector_3d<T>::operator+(const vector_3d<T>& v2) const
{
    vector_3d<T> result(*this);
    return result += v2;
}

template <typename T>
vector_3d<T>& vector_3d<T>::operator-=(const vector_3d<T>& v2)
{
    a = a - v2.a;
    b = b - v2.b;
    c = c - v2.c;
    return *this;
}

template <typename T>
vector_3d<T>& vector_3d<T>::operator-(const vector_3d<T>& v2) const
{
    vector_3d<T> result(*this);
    return result -= v2;
}

template <typename T>
vector_3d<T>& vector_3d<T>::operator*=(const T s2)
{
    a = a * s2;
    b = b * s2;
    c = c * s2;
    return *this;
}

template <typename T>
vector_3d<T>& vector_3d<T>::operator/=(const T s2)
{
    //if (s2 == 0) throw
    a = a / s2;
    b = b / s2;
    c = c / s2;
    return *this;
}

template <typename T>
T vector_3d<T>::dot(const vector_3d<T>& v2)
{
    return (a*v2.a + b*v2.b + c*v2.c);
}


// template <typename T>
// void vector_3d<T>::cross_product(const vector_3d v2)
// {
//     a = b*v2.c - c*v2.b;
//     b = c*v2.a - a*v2.c; // deadly wrong!!!!
//     c = a*v2.b - b*v2.a;
// }

template <typename T>
vector_3d<T> vector_3d<T>::cross(const vector_3d &v2) const
{
    vector_3d tmp;
    tmp.a = b*v2.c - c*v2.b;
    tmp.b = c*v2.a - a*v2.c;
    tmp.c = a*v2.b - b*v2.a;
    return tmp;
}

template <typename T>
void vector_3d<T>::normalize()
{
    T one_by_sqrt;
    one_by_sqrt = 1/get_norm();
    a = a*one_by_sqrt;
    b = b*one_by_sqrt;
    c = c*one_by_sqrt;
}

template class vector_3d<float>; // explicit instantiation
template class vector_3d<double>;
