#include "NObject3d.h"
#include <cassert>

void NObject3d::LoadFile()
{
	//ファイルストリーム
	std::ifstream file;
	//.objファイルを開く
	file.open("Resource/triangle/triangle.obj");
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	vector<XMFLOAT3>positions;	//頂点座標
	vector<XMFLOAT3>normals;	//法線ベクトル
	vector<XMFLOAT3>texcoords;	//テクスチャUV
	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');
	}
	//ファイル閉じる
	file.close();
}
