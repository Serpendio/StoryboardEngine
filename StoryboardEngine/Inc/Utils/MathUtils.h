#pragma once
#include "pch.h"

namespace MathUtils
{
	constexpr float PI = DirectX::XM_PI;
	constexpr float TWO_PI = DirectX::XM_2PI;

	inline float DegToRad (float degrees)
	{
		return degrees * (PI / 180.0f);
	}

	inline Vector2 DegToRad(const Vector2& degrees)
	{
		return Vector2(DegToRad(degrees.x), DegToRad(degrees.y));
	}

	inline Vector3 DegToRad(const Vector3& degrees)
	{
		return Vector3(DegToRad(degrees.x), DegToRad(degrees.y), DegToRad(degrees.z));
	}

	inline float RadToDeg(float radians)
	{
		return radians * (180.0f / PI);
	}

	inline Vector2 RadToDeg(const Vector2& radians)
	{
		return Vector2(RadToDeg(radians.x), RadToDeg(radians.y));
	}

	inline Vector3 RadToDeg(const Vector3& radians)
	{
		return Vector3(RadToDeg(radians.x), RadToDeg(radians.y), RadToDeg(radians.z));
	}

	inline bool Approximately(float a, float b, float epsilon = 0.0001f)
	{
		return fabsf(a - b) <= epsilon;
	}
}

