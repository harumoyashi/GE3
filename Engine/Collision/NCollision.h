#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include "NVector2.h"
#include "NVector3.h"

class NCollision
{
public:
	struct Sphere
	{
		NVector3 pos = { 0.0f,0.0f,0.0f };	//座標
		float radius = 1.0f;	//球の半径
	};

	struct Plane
	{
		NVector3 normal = { 0.0f,1.0f,0.0f };	//法線
		float distance = 0.0f;	//原点からの距離
	};

	//球同士の当たり判定
	//col:当たり判定参照するやつ
	static bool SphereCol(const Sphere& s0, const Sphere& s1);

	static bool Sphere2PlaneCol(const Sphere& sphere,const Plane& plane/*,NVector3 inter*/);
};