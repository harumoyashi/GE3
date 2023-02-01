#include "NVector2.h"
#include <cmath>    // sqrt

NVector2::NVector2() :x(0), y(0) {}

NVector2::NVector2(float x, float y) : x(x), y(y) {}

float NVector2::Length() const
{
	return sqrtf(x * x + y * y);
}

NVector2& NVector2::Normalize()
{
	float len = Length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float NVector2::Dot(const NVector2& v) const
{
	return x * v.x + y * v.y;
}

float NVector2::Cross(const NVector2& v) const
{
	return x * v.y - y * v.x;
}

NVector2 NVector2::operator+() const
{
	return *this;
}

NVector2 NVector2::operator-() const
{
	return NVector2(-x, -y);
}

NVector2& NVector2::operator+=(const NVector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

NVector2& NVector2::operator-=(const NVector2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

NVector2& NVector2::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

NVector2& NVector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

// NVector2 ÉNÉâÉXÇ…ëÆÇ≥Ç»Ç¢ä÷êîåR
// ìÒçÄââéZéq
const NVector2 operator+(const NVector2& v1, const NVector2& v2)
{
	NVector2 temp(v1);
	return temp += v2;
}

const NVector2 operator-(const NVector2& v1, const NVector2& v2)
{
	NVector2 temp(v1);
	return temp -= v2;
}

const NVector2 operator*(const NVector2& v, float s)
{
	NVector2 temp(v);
	return temp *= s;
}

const NVector2 operator*(float s, const NVector2& v)
{
	return v * s;
}

const NVector2 operator/(const NVector2& v, float s)
{
	NVector2 temp(v);
	return temp /= s;
}