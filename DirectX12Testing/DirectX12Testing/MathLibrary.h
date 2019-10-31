#pragma once
#include<DirectXMath.h>

class MathLibrary
{
public:
	// Clamp a value between a min and max range.
	template<typename T>
	inline constexpr const T& clamp(const T& val, const T& min, const T& max)
	{
		return val < min ? min : val > max ? max : val;
	}



};