#pragma once

#include <filesystem>

bool IsStringEqual(cstring left, cstring right);
float Saturate(float x);
float Lerp(float start, float end, float t);
float GetRandomFloat();
float DegreesToRadians(float degrees);

vector<uchar> ReadFile(std::filesystem::path filePath, bool addNullTerminator = false);

#ifdef STATS
	struct Stats {
		int primaryRayCounter = 0;
		int sphereRayIntersectionCounter = 0;
		int aabbRayIntersectionCounter = 0;
	};
	inline Stats* stats;
#endif