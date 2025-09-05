#pragma once

#include <filesystem>

bool streq(cstring left, cstring right);
vector<uchar> ReadFile(std::filesystem::path file_path, bool addNullTerminator = false);