#pragma once
#include <math.h>
#include "NVector2.h"
#include "NVector3.h"
#include "NMatrix4.h"
#include "NQuaternion.h"

namespace MathUtil
{
	//ビュー変換行列生成
	NMatrix4 MatView(NVector3 eye, NVector3 target, NVector3 up);
	//平行投影変換行列生成
	NMatrix4 ParallelProjection(float WIN_WIDTH, float WIN_HEIGHT);
	//透視投影変換行列生成
	NMatrix4 PerspectiveProjection(float fov, float aspect, float nearZ, float farZ);

	//度数法→ラジアン
	float Degree2Radian(float degree);
	//度数法→ラジアン
	float Radian2Degree(float radian);
	//符号返す
	int Sign(int value);
	//符号返す
	float Sign(float value);
	//最小、最大値設定
	float Clamp(float value, float max, float min);
}