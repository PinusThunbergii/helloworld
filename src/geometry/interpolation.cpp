#include "interpolation.h"

Vec3f calculateLinearSystem(mat<3, 3, float> &A, Vec3f &b)
{
    Vec3f x = A.invert() * b;
    return x;
}

Vec4f calculateLinearSystem(mat<4, 4, float> &A, Vec4f &b)
{
    Vec4f x = A.invert() * b;
    return x;
}

float cubicInterpolation(float *v_p, float *v_c, float p)
{
    mat<4, 4, float> A;
    Vec4f b;
    for(int i = 0; i < 4; i++)
    {
        Vec4f row;
        for(int j = 0; j < 4; j++)
            row[j] = 1;
        for(int k = 0; k < 4; k++)
            for(int n = 3 - k; n > 0 ; n--)
                row[k] *= v_c[i];
        A[i] = row;
    }
    for(int i = 0; i < 4; i++)
        b[i] = v_p[i];
   
    Vec4f X = calculateLinearSystem(A, b);
    
    //calculate N{X^3, X^2, X^1, X^0}
    Vec4f N;
    for(int j = 0; j < 4; j++)
            N[j] = 1;
        for(int k = 0; k < 4; k++)
            for(int n = 3 - k; n > 0 ; n--)
                N[k] *= p;

    float result = X * N;      
    return result;
}

Vec3f cubicInterpolation(Vec3f *v_p, float *v_c, float p)
{
    Vec3f result;
    for(int i = 0; i < 3; i++)
    {
        float tmp[4] = {v_p[0][i], v_p[1][i], v_p[2][i], v_p[3][i]};
        result[i] = cubicInterpolation(tmp, v_c, p);
    }
    return result;
}