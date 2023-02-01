#include "NCollision.h"

bool NCollision::SphereCol(const Sphere& s0, const Sphere& s1)
{
	float distX = (s0.pos.x - s1.pos.x) * (s0.pos.x - s1.pos.x);
	float distY = (s0.pos.y - s1.pos.y) * (s0.pos.y - s1.pos.y);
	float distZ = (s0.pos.z - s1.pos.z) * (s0.pos.z - s1.pos.z);
	float distR = (s0.radius + s1.radius) * (s0.radius + s1.radius);

	if (distX + distY + distZ <= distR)
	{
		return true;
	}
	return false;
}

bool NCollision::Sphere2PlaneCol(const Sphere& sphere, const Plane& plane/*, NVector3 inter*/)
{
	//座標系の原点から球の中心座標への距離
	float distV = sphere.pos.Dot(plane.normal);

	//平面の原点距離を減算することで、平面と球の中心との距離を出す
	float dist = distV - plane.distance;

	//距離の絶対値が半径より大きければ当たってない
	if (fabsf(dist) > sphere.radius)
	{
		return false;
	}

	return true;
}