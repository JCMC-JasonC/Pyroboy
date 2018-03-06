#pragma once

#include <glm/vec3.hpp>

namespace Math
{
	// Linear interpolation (LERP)
	template <typename T>
	T lerp(T d0, T d1, float t)
	{
		return (1 - t) * d0 + d1 * t;
	}

}