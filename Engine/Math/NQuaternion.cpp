#include "NQuaternion.h"
#include <math.h>

NQuaternion NQuaternion::operator+(const NQuaternion& q) const
{
	return NQuaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

NQuaternion NQuaternion::operator-(const NQuaternion& q) const
{
	return NQuaternion(x - q.x, y - q.y, z - q.z, w - q.w);
}

NQuaternion NQuaternion::operator*(const NQuaternion& q) const
{
	return NQuaternion(
		w * q.x - z * q.y + y * q.z + x * q.w,
		z * q.x + w * q.y - x * q.z + y * q.w,
		-y * q.x + x * q.y + w * q.z + z * q.w,
		-x * q.x - y * q.y - z * q.z + w * q.w
	);
}

NQuaternion NQuaternion::operator+=(const NQuaternion& q)
{
	NQuaternion result = *this;
	result = result + q;
	*this = result;
	return *this;
}

NQuaternion NQuaternion::operator-=(const NQuaternion& q)
{
	NQuaternion result = *this;
	result = result - q;
	*this = result;
	return *this;
}

NQuaternion& NQuaternion::operator*=(const NQuaternion& q)
{
	NQuaternion result = *this;
	result = result * q;
	*this = result;
	return *this;
}

NQuaternion NQuaternion::operator-() const
{
	return NQuaternion{ -x,-y,-z,-w };
}

NQuaternion NQuaternion::operator*(const float& s) const
{
	return NQuaternion(x * s, y * s, z * s, w * s);
}

NQuaternion NQuaternion::operator/(const float& s) const
{
	return NQuaternion(x / s, y / s, z / s, w / s);
}

NQuaternion& NQuaternion::operator*=(const float& s)
{
	NQuaternion result = *this;
	result = result * s;
	*this = result;
	return *this;
}

NQuaternion& NQuaternion::operator/=(const float& s)
{
	NQuaternion result = *this;
	result = result / s;
	*this = result;
	return *this;
}

NQuaternion NQuaternion::Mul(const NQuaternion& lhs, const NQuaternion& rhs)
{
	NQuaternion lq = lhs;
	NQuaternion rq = rhs;

	NQuaternion result(
		lq.w * rq.x - lq.z * rq.y + lq.y * rq.z + lq.x * rq.w,
		lq.z * rq.x + lq.w * rq.y - lq.x * rq.z + lq.y * rq.w,
		-lq.y * rq.x + lq.x * rq.y + lq.w * rq.z + lq.z * rq.w,
		-lq.x * rq.x - lq.y * rq.y - lq.z * rq.z + lq.w * rq.w
	);
	return result;
}

NQuaternion NQuaternion::Identity()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 1;
	return *this;
}

NQuaternion NQuaternion::Conjugate(const NQuaternion& q)
{
	NQuaternion result = q;
	//虚部を全て反転
	result.x = -result.x;
	result.y = -result.y;
	result.z = -result.z;
	return result;
}

float NQuaternion::GetNorm() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

float NQuaternion::Norm(const NQuaternion& q)
{
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

float NQuaternion::Dot(const NQuaternion& q0, const NQuaternion& q1)
{
	return q0.w * q1.w + q0.x * q1.x + q0.y * q1.y + q0.z * q1.z;
}

NVector3 NQuaternion::Cross(const NQuaternion& q0, const NQuaternion& q1)
{
	float x = q0.y * q1.z - q0.z * q1.y;
	float y = q0.z * q1.x - q0.x * q1.z;
	float z = q0.x * q1.y - q0.y * q1.x;

	return { x,y,z };
}

NQuaternion NQuaternion::Normalize()
{
	float norm = Norm(*this);
	*this /= norm;
	return *this;
}

NQuaternion NQuaternion::Normalize(const NQuaternion& q)
{
	NQuaternion result = q;
	float norm = Norm(result);
	result /= norm;
	return result;
}

NQuaternion NQuaternion::Inverse(const NQuaternion& q)
{
	NQuaternion result = q;
	float norm = Norm(result);
	result = Conjugate(result);	//共役を
	result /= (norm * norm);	//normの2乗(スカラー)で割ると逆クォータニオン
	return result;
}

NQuaternion NQuaternion::MakeAxisAngle(const NVector3& axis, const float& angle)
{
	NQuaternion q(
		axis.x * sinf(angle / 2.0f),
		axis.y * sinf(angle / 2.0f),
		axis.z * sinf(angle / 2.0f),
		cosf(angle / 2.0f)
	);
	q.Normalize();
	return q;
}

NQuaternion NQuaternion::EulerToQuaternion(const NVector3& rot) const
{
	NQuaternion x = MakeAxisAngle({ 1, 0, 0 }, rot.x);
	NQuaternion y = MakeAxisAngle({ 0, 1, 0 }, rot.y);
	NQuaternion z = MakeAxisAngle({ 0, 0, 1 }, rot.z);

	NQuaternion q = z * x * y;

	return q;
}

NMatrix4 NQuaternion::QuaternionToMatrix() const
{
	NMatrix4 mat;
	mat = mat.Identity();

	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float ww = w * w;
	float xy2 = x * y * 2;
	float xz2 = x * z * 2;
	float xw2 = x * w * 2;
	float yz2 = y * z * 2;
	float yw2 = y * w * 2;
	float zw2 = z * w * 2;

	mat.m[0][0] = ww + xx - yy - zz;
	mat.m[0][1] = xy2 + zw2;
	mat.m[0][2] = xz2 - yw2;
	mat.m[1][0] = xy2 - zw2;
	mat.m[1][1] = ww - xx + yy - zz;
	mat.m[1][2] = yz2 + xw2;
	mat.m[2][0] = xz2 + yw2;
	mat.m[2][1] = yz2 - xw2;
	mat.m[2][2] = ww - xx - yy + zz;
	return mat;
}

NVector3 NQuaternion::RotateVector(const NVector3& vector, const NQuaternion& quaternion) const
{
	NMatrix4 mat;
	mat = quaternion.QuaternionToMatrix();

	NVector3 vec;
	vec = mat.ToEuler();

	NMatrix4 mat2;
	mat2 *= mat2.RotateZ(vec.z);
	mat2 *= mat2.RotateX(vec.x);
	mat2 *= mat2.RotateY(vec.y);

	return vector * mat;
}

NQuaternion NQuaternion::Slarp(const NQuaternion& q0, const NQuaternion& q1, float t)
{
	float dot = Dot(q0, q1);
	NQuaternion q = q0;
	if (dot < 0)
	{
		q = -q;
		dot = -dot;
	}

	float scale0 = 1.0f - t;
	float scale1 = t;
	if ((1.0f - dot) > 0.001f)
	{
		float theta = acosf(dot);
		scale0 = sinf(theta * scale0) / sinf(theta);
		scale1 = sinf(theta * scale1) / sinf(theta);
	}

	if (dot >= 1.0f - 0.00001f)
	{
		return q * scale0 + q1 * scale1;
	}
	return q * (1.0f - t) + q1 * t;
}

NQuaternion NQuaternion::VecToVecRota(const NVector3& u, const NVector3& v)
{
	//正規化して内積を求める
	float dot = u.Normalize().Dot(v.Normalize());

	//軸を求める(外積の結果が単位ベクトルとは限らないので正規化必須)
	NVector3 axis = u.Cross(v).Normalize();

	//単位ベクトルで内積を取ったのでacosで角度を求める
	float theta = acosf(dot);

	//求めた軸と角度を使って任意軸回転を返す
	return MakeAxisAngle(axis, theta);
}
