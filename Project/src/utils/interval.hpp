#pragma once

struct Interval {
	Interval() : min(INFINITY), max(-INFINITY) {}
	Interval(float min, float max) : min(min), max(max) {}

	float min;
	float max;

	float Size() const;
	bool Contains(float x) const;
	bool Surrounds(float x) const;
};

inline float Interval::Size() const {
	return max - min;
}

inline bool Interval::Contains(float x) const {
	return min <= x && x <= max;
}

inline bool Interval::Surrounds(float x) const {
	return min < x && x < max;
}