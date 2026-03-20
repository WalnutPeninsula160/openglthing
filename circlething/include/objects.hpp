// definitions for all the boring functions involved in the creation of objects
#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include "objects.hpp"

// type definitions
template <size_t segments>
using circleObject = std::array<float, (segments+1) << 1>;

#define circle_vert_data(x, y, r, segs) generate_circle_vert_data<segs>(x, y, r)
template <size_t segments>
constexpr std::array<float, (segments+1) << 1> generate_circle_vert_data(float x, float y, float r) {
	float jump = 2 * static_cast<float>(M_PI) / segments;
	float angle = 0.f;
	std::array<float, (segments+1) << 1> result;
	result.fill(0.f);
	for (size_t i = 1; i < segments+1; i++) {
		result[i << 1] = y + r * std::cosf(angle);
		result[(i << 1) + 1] = x + r * std::sinf(angle);
		angle += jump;
	}
	return result;
}
