#include "NCube.h"

////静的メンバ変数の実体
//std::vector<Vertex> NCube::vertices;
//std::vector<unsigned short>NCube::indices;


void NCube::Init(int modelNum)
{
	//頂点
	if (modelNum == CUBE)
	{
		SetVertCube();
		SetIndexCube();	//ここでインデックスの設定だけ行う
	}
	else if (modelNum == CRYSTAL)
	{
		SetVertCrystal();
		SetIndexCrystal();	//ここでインデックスの設定だけ行う
	}
	vertexBuff.Init(vertices);
	indexBuff.Init(indices);
}

void NCube::SetVertCube()
{
	// 頂点データ設定
	vertices = {
		//	x		y		z	 法線	u	v
		//前
		{{ -1.0f, -1.0f, -1.0f }, {}, {0.0f,1.0f}},	// 左下
		{{ -1.0f,  1.0f, -1.0f }, {}, {0.0f,0.0f}},	// 左上
		{{  1.0f, -1.0f, -1.0f }, {}, {1.0f,1.0f}},	// 右下
		{{  1.0f,  1.0f, -1.0f }, {}, {1.0f,0.0f}},	// 右上

		//後
		{{ -1.0f, -1.0f, 1.0f }, {}, {0.0f,1.0f}},	// 左下
		{{ -1.0f,  1.0f, 1.0f }, {}, {0.0f,0.0f}},	// 左上
		{{  1.0f, -1.0f, 1.0f }, {}, {1.0f,1.0f}},	// 右下
		{{  1.0f,  1.0f, 1.0f }, {}, {1.0f,0.0f}},	// 右上

		 // 左
		{{-1.0f,-1.0f,-1.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{-1.0f,-1.0f, 1.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{-1.0f, 1.0f,-1.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{-1.0f, 1.0f, 1.0f }, {}, {1.0f, 0.0f}},    // 右上

		// 右
		{{ 1.0f,-1.0f,-1.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{ 1.0f,-1.0f, 1.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{ 1.0f, 1.0f,-1.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{ 1.0f, 1.0f, 1.0f }, {}, {1.0f, 0.0f}},    // 右上

		// 上
		{{-1.0f,-1.0f,-1.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{ 1.0f,-1.0f,-1.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{-1.0f,-1.0f, 1.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{ 1.0f,-1.0f, 1.0f }, {}, {1.0f, 0.0f}},    // 右上

		// 下
		{{-1.0f, 1.0f,-1.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{ 1.0f, 1.0f,-1.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{-1.0f, 1.0f, 1.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{ 1.0f, 1.0f, 1.0f }, {}, {1.0f, 0.0f}},    // 右上
	};

	////頂点バッファのサイズを代入
	//singleSizeVB = static_cast<UINT>(sizeof(vertices[0]));
	//sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
}

void NCube::SetVertCrystal()
{
	// 頂点データ設定
	vertices = {
		//	x		y		z	 法線	u	v
			//左前
		{ { -0.0f, +2.0f, -0.0f }, {}, { 0.0f,0.0f }},	// 上
		{ {  0.0f, -0.0f, -1.0f }, {}, {0.0f,0.0f} },	// 右下
		{ { -1.0f,  0.0f, -0.0f }, {}, {0.0f,0.0f} },	// 左下
		{ { -0.0f, -2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 下

		//右前
		{ { -0.0f, +2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 上
		{ { +1.0f,  0.0f, -0.0f }, {}, {0.0f,0.0f} },	// 右下
		{ {  0.0f, -0.0f, -1.0f }, {}, {0.0f,0.0f} },	// 左下
		{ { -0.0f, -2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 下

		//左後
		{ { -0.0f, +2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 上
		{ { -1.0f,  0.0f, -0.0f }, {}, {0.0f,0.0f} },	// 右下
		{ { -0.0f,  0.0f, +1.0f }, {}, {0.0f,0.0f} },	// 左下
		{ { -0.0f, -2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 下

		//右後
		{ { -0.0f, +2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 上
		{ { -0.0f,  0.0f, +1.0f }, {}, {0.0f,0.0f} },	// 右下
		{ { +1.0f,  0.0f, -0.0f }, {}, {0.0f,0.0f} },	// 左下
		{ { -0.0f, -2.0f, -0.0f }, {}, {0.0f,0.0f} },	// 下
	};

	////頂点バッファのサイズを代入
	//singleSizeVB = static_cast<UINT>(sizeof(vertices[0]));
	//sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
}

void NCube::SetIndexCube()
{
	//インデックスデータ
	indices =
	{
		//前
		0,1,2,	//三角形1つ目
		2,1,3,	//三角形2つ目
		//後
		5,4,6,	//三角形3つ目
		5,6,7,	//三角形4つ目
		//左
		8,9,10,	//三角形5つ目
		10,9,11,//三角形6つ目
		//右
		13,12,14,	//三角形7つ目
		13,14,15,	//三角形8つ目
		//下
		16,17,18,	//三角形9つ目
		18,17,19,	//三角形10つ目
		//上
		21,20,22,	//三角形11つ目
		21,22,23,	//三角形12つ目
	};

	////頂点バッファのサイズを代入
	//sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//numIB = static_cast<UINT>(sizeof(unsigned short) * indices.size() / sizeof(indices[0]));
}

void NCube::SetIndexCrystal()
{
	//インデックスデータ
	indices =
	{
		//左前
		0, 1, 2,	//三角形1つ目
		2, 1, 3,	//三角形2つ目
		//右前
		4, 5, 6,	//三角形3つ目
		6, 5, 7,	//三角形4つ目
		//左後
		8, 9, 10,	//三角形5つ目
		10, 9, 11,	//三角形6つ目
		//右後
		12, 13, 14,	//三角形7つ目
		14, 13, 15,	//三角形8つ目
	};

	////頂点バッファのサイズを代入
	//sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//numIB = static_cast<UINT>(sizeof(unsigned short) * indices.size() / sizeof(indices[0]));
}