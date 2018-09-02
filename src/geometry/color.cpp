#include "color.h"

HSV RGB2HSV(RGB rgb)
{
    float r = static_cast<float>(rgb.R) / 255.0f;
    float g = static_cast<float>(rgb.G) / 255.0f; 
    float b = static_cast<float>(rgb.B) / 255.0f;

    float Max = std::max(std::max(r, g), b);
    float Min = std::min(std::min(r, g), b);
    float Delta = Max - Min;

    HSV converted;
    //const float epsilon = std::numeric_limits<float>::min() * 4.0f;
    if(Delta == 0.0f)
        converted.H = 0.0f;
    else if(Max == r)
        converted.H = 60.0f * fmod((g - b) / Delta, 6.0f);
    else if(Max == g)
        converted.H = 60.0f * ((b - r) / Delta + 2.0f);
    else
        converted.H = 60.0f * ((r - g) / Delta + 4.0f);    
    
    if(Max == 0.0f)
        converted.S = 0.0f;
    else
        converted.S = Delta / Max;

    converted.V = Max;   
    return converted;
}

RGB HSV2RGB(HSV hsv)
{
    float C = hsv.V * hsv.S;
    float X = C * (1.0f - fabs(fmod(hsv.H / 60.0f, 2.0f) - 1.0f));
    float m = hsv.V - C;

    float r, g, b;
    if(hsv.H >= 0.0f && hsv.H < 60.0f)
    {
        r = C; g = X; b = 0.0f;
    }
    else if(hsv.H >= 60.0f && hsv.H < 120.0f)
    {
        r = X; g = C; b = 0.0f;
    }
    else if(hsv.H >= 120.0f && hsv.H < 180.0f)
    {
        r = 0.0f; g = C; b = X;
    }
    else if(hsv.H >= 180.0f && hsv.H < 240.0f)
    {
        r = 0; g = X; b = C;
    }
    else if(hsv.H >= 240.0f && hsv.H < 300.0f)
    {
        r = X; g = 0.0f; b = C;
    }
    else if(hsv.H >= 300.0f && hsv.H < 360.0f)
    {
        r = C; g = 0.0f; b = X;
    }
    RGB converted;
    converted.R = static_cast<uint8_t>((r + m) * 255.0f);
    converted.G = static_cast<uint8_t>((g + m) * 255.0f);
    converted.B = static_cast<uint8_t>((b + m) * 255.0f);
    return converted;
}

Vec3f RGB2HSV(Vec3f rgb)
{
    RGB a(rgb[0], rgb[1], rgb[2]);
    HSV b = RGB2HSV(a);
    return Vec3f(b.H, b.S, b.V);
}

Vec3f HSV2RGB(Vec3f hsv)
{
    HSV a(hsv[0], hsv[1], hsv[2]);
    RGB b = HSV2RGB(a);
    return Vec3f(b.R, b.G, b.B);
}