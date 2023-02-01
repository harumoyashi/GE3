#pragma once
#include <d3d12.h>
#include <vector>
#include "NDX12.h"
#include "NVector2.h"
#include "NVector3.h"

class NVertex
{
public:
	NVector3 pos = { 0, 0, 0 };

	NVertex(NVector3 pos = { 0, 0, 0 })
		: pos(pos) {}
};

class NVertexPNU
{
public:
	NVector3 pos = { 0, 0, 0 };
	NVector3 normal = { 0, 0, 1 };
	NVector2 uv = { 0, 0 };

	NVertexPNU(NVector3 pos = { 0, 0, 0 }, NVector3 normal = { 0, 0, 1 }, NVector2 uv = { 0, 0 })
		: pos(pos), normal(normal), uv(uv) {}

	bool operator==(const NVertexPNU& a) const;

	/// <summary>
	/// 法線ベクトルを計算
	/// </summary>
	/// <param name="vertices">計算する頂点群</param>
	/// <param name="indices">インデックスリスト（三角形、全て時計回りであること）</param>
	static void CalcNormalVec(std::vector<NVertexPNU> vertices, std::vector<unsigned int> indices);
};