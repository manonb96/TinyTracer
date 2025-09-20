#pragma once

#include <filesystem>

bool IsStringEqual(cstring left, cstring right);
float Saturate(float x);
float Lerp(float start, float end, float t);
float GetRandomFloat();

vector<uchar> ReadFile(std::filesystem::path filePath, bool addNullTerminator = false);