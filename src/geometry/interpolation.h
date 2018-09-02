#ifndef _INTERPOLATION_
#define _INTERPOLATION_
#include <math.h>
#include <iostream>
#include "geometry.h"

template<typename T, typename M>
T linearInterpolation(T v0_p, T v1_p, M v0_c, M v1_c, M p)
{
    //return a + (b - a) * t; 
    //return a*(1 - t) + b * t; 		int p = v0.y + (x - v0.x) * int(v1.y - v0.y) / int(v1.x - v0.x);

    return v0_p + (v1_p - v0_p) * (float(p - v0_c) / float( v1_c - v0_c));
}

template<typename T, typename M>
T bilinearInterpolation(T v00_p, T v01_p, T v11_p, T v10_p, M v00_c, M v01_c, M v11_c, M v10_c, M p)
{
    T g = linearInterpolation<T, int>(v00_p, v10_p, v00_c[0], v10_c[0], p[0]);
    T n = linearInterpolation<T, int>(v01_p, v11_p, v01_c[0], v11_c[0], p[0]);
    T m = linearInterpolation<T, int>(g, n, v00_c[1], v01_c[1], p[1]);
    return m;
}

Vec3f calculateLinearSystem(mat<3, 3, float> &A, Vec3f &b);
Vec4f calculateLinearSystem(mat<4, 4, float> &A, Vec4f &b);

float cubicInterpolation(float *v_p, float *v_c, float p);
Vec3f cubicInterpolation(Vec3f *v_p, float *v_c, float p);

#endif