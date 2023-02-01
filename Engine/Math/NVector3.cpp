#include "NVector3.h"
#include <cmath>

NVector3::NVector3():x(0),y(0),z(0) {

}

NVector3::NVector3(float x, float y, float z) :x(x), y(y), z(z) {

}

float NVector3::Length() const {
	return sqrtf(x * x + y * y + z * z);
}

NVector3 NVector3::Normalize() const {
	float len = Length();
	if (len != 0)
	{
		return *this / len;
	}
	return *this;
}

NVector3 NVector3::Cross(const NVector3& v) const
{
	NVector3 vec3;
	vec3.x = y * v.z - z * v.y;
	vec3.y = z * v.x - x * v.z;
	vec3.z = x * v.y - y * v.x;

	return vec3;
}

float NVector3::Dot(const NVector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

NVector3 NVector3::operator+()const
{
	return *this;
}

NVector3 NVector3::operator-()const
{
	return NVector3(-x, -y, -z);
}

NVector3 NVector3::operator*(const NVector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

NVector3 NVector3::operator/(const NVector3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

NVector3& NVector3::operator+=(const NVector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

NVector3& NVector3::operator-=(const NVector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

NVector3& NVector3::operator*=(const NVector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

NVector3& NVector3::operator/=(const NVector3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

NVector3& NVector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

NVector3& NVector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

NVector3& NVector3::operator=(float num)
{
	x = num;
	y = num;
	z = num;

	return *this;
}

//NVector3ÉNÉâÉXÇ…ëÆÇ≥Ç»Ç¢ä÷êîåQ
// ìÒçÄââéZéq
const NVector3 operator+(const NVector3& v1, const NVector3& v2)
{
	NVector3 temp(v1);
	return temp += v2;
}

const NVector3 operator-(const NVector3& v1, const NVector3& v2)
{
	NVector3 temp(v1);
	return temp -= v2;
}

const NVector3 operator*(const NVector3& v, float s)
{
	NVector3 temp(v);
	return temp *= s;
}

const NVector3 operator*(float s, const NVector3& v)
{
	return v * s;
}

const NVector3 operator/(const NVector3& v, float s)
{
	NVector3 temp(v);
	return temp /= s;
}