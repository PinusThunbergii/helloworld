#ifndef _COLOR_
#define _COLOR_

#include <iostream>
#include <algorithm>
#include <limits>
#include <math.h>
#include "geometry.h"

struct RGB
{
    union
    {
        struct
        {
            uint8_t R, G, B;
        };
        uint8_t raw[3];
    };
    RGB() = default;
    RGB(uint8_t R, uint8_t G, uint8_t B) : R(R), G(G), B(B) {};
};

struct HSV
{
    union
    {
        struct
        {
            float H, S, V;
        };
        float raw[3];
    };
    HSV() = default;
    HSV(float H, float S, float V) : H(H), S(S), V(V) {};
};

HSV RGB2HSV(RGB rgb);

RGB HSV2RGB(HSV hsv);

Vec3f RGB2HSV(Vec3f rgb);
Vec3f HSV2RGB(Vec3f hsv);

#endif