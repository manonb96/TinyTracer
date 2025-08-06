#pragma once

#include <filesystem>

bool streq(gsl::czstring left, gsl::czstring right);
std::vector<std::uint8_t> ReadFile(std::filesystem::path file_path);