#pragma once

#include <filesystem>

struct Color;

bool streq(cstring left, cstring right);
float saturate(float x);
float lerp(float start, float end, float t);
Color lerp(Color start, Color end, float t);

vector<uchar> ReadFile(std::filesystem::path file_path, bool addNullTerminator = false);