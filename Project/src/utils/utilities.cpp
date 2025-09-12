#include "utilities.hpp"
#include <fstream>
#include <spdlog/spdlog.h>

bool streq(cstring left, cstring right) {
	return std::strcmp(left, right) == 0;
}

float lerp(float start, float end, float t)
{
	return start + t * (end - start);
}

vector<uchar> ReadFile(std::filesystem::path file_path, bool addNullTerminator) {
	if (!std::filesystem::exists(file_path)) {
		spdlog::error("No file found at {}", file_path.string());
		return {};
	}

	if (!std::filesystem::is_regular_file(file_path)) {
		spdlog::error("File at {} is not a regular file", file_path.string());
		return {};
	}

	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open()) {
		spdlog::error("Could not open file at {}", file_path.string());
		return {};
	}

	const uint size = std::filesystem::file_size(file_path);
	vector<uchar> buffer(size);
	file.read(reinterpret_cast<char*>(buffer.data()), size);

	if (addNullTerminator)
	{
		buffer.push_back(0);
	}

	return buffer;
}

