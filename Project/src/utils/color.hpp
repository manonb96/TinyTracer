#pragma once

struct Color {
    Color();
    Color(float r, float g, float b);
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
inline Color::Color(float3 rgb, float a) : rgb(rgb), a(a) {}

inline void WriteColor(uchar* pixels, int x, int y, const Color& pixel_color)
{
    int offset = (y * WIDTH + x) * 4;
    pixels[offset + 0] = (int)(pixel_color.r * 255.f);
    pixels[offset + 1] = (int)(pixel_color.g * 255.f);
    pixels[offset + 2] = (int)(pixel_color.b * 255.f);
    pixels[offset + 3] = (int)(pixel_color.a * 255.f);
}

static const Color COLOR_BLACK = Color(0.0f, 0.0f, 0.0f);