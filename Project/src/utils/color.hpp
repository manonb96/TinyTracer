#pragma once
#include "utilities.hpp"

struct Color {
    Color();
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    Color(float3 rgb, float a);
    union {
        struct {
            float r, g, b;
        };
        float3 rgb;
    };
    float a;
};

inline Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
inline Color::Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
inline Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
inline Color::Color(float3 rgb, float a) : rgb(rgb), a(a) {}
inline Color operator+(Color firstColor, Color secondColor)
{
    return Color(firstColor.r + secondColor.r, firstColor.g + secondColor.g, firstColor.b + secondColor.b, firstColor.a + secondColor.a);
}
inline Color operator*(float scalar, Color color) { return Color(scalar * color.r, scalar * color.g, scalar * color.b, color.a); } // NOTE: alpha is not scaled
inline Color operator/(Color color, float s)
{
    return Color(color.r / s, color.g / s, color.b / s, color.a / s);
}
inline Color& operator+=(Color& firstColor, const Color& secondColor) {
    firstColor.r += secondColor.r; firstColor.g += secondColor.g; firstColor.b += secondColor.b; firstColor.a += secondColor.a; return firstColor;
}
inline Color& operator*=(Color& color, const float s) {
    color.r *= s; color.g *= s; color.b *= s; color.a *= s; return color;
}
inline Color& operator/=(Color& color, const float s) {
    color.r /= s; color.g /= s; color.b /= s; color.a /= s; return color;
}
inline Color Clamp(const Color& pixel_value)
{
    float r = saturate(pixel_value.r);
    float g = saturate(pixel_value.g);
    float b = saturate(pixel_value.b);
    float a = saturate(pixel_value.a);

    return Color(r, g, b, a);
}

inline void WriteColor(uchar* pixels, int x, int y, const Color& pixel_value)
{
    Color clampedColor = Clamp(pixel_value);

    int offset = (y * IMAGE_WIDTH + x) * 4;
    pixels[offset + 0] = (int)(clampedColor.r * 255.f);
    pixels[offset + 1] = (int)(clampedColor.g * 255.f);
    pixels[offset + 2] = (int)(clampedColor.b * 255.f);
    pixels[offset + 3] = (int)(clampedColor.a * 255.f);
}

static const Color COLOR_BLACK = Color(0.0f, 0.0f, 0.0f);
static const Color COLOR_WHITE = Color(1.0f, 1.0f, 1.0f);
static const Color COLOR_RED = Color(1.0f, 0.0f, 0.0f);
static const Color COLOR_LIGHTBLUE = Color(0.5, 0.7, 1.0);