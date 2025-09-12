#pragma once

#include <filesystem>

bool streq(cstring left, cstring right);
float lerp(float start, float end, float t);

vector<uchar> ReadFile(std::filesystem::path file_path, bool addNullTerminator = false);