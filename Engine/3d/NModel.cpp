#include "NModel.h"
#include "NMtllib.h"

#include <fstream>
#include <sstream>
#include <string>
using namespace std;

void NModel::Create(const string modelname)
{
	LoadObjFile(modelname);
	vertexBuff.Init(vertices);
	indexBuff.Init(indices);
}

void NModel::LoadObjFile(const string name)
{
	//ファイルストリーム
	std::ifstream file;
	//.objファイルを開く
	const string modelname = name;
	const string filename = modelname + ".obj";	//"triangle_mat.obj"
	const string directoryPath = "Resources/" + modelname + "/";	//"Resources/triangle_mat/
	file.open(directoryPath + filename);	//"Resources/triangle_mat/triangle_mat.obj"
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	vector<NVector3>positions;	//頂点座標
	vector<NVector3>normals;	//法線ベクトル
	vector<NVector2>texcoords;	//テクスチャUV
	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭文字列がmtllibならマテリアル
		if (key == "mtllib")
		{
			//マテリアルのファイル名読み込み
			string filename;
			line_stream >> filename;
			//マテリアル読み込み
			material = NMtllib::Load(directoryPath,filename);
		}

		//先頭文字列がvなら頂点座標
		if (key == "v")
		{
			//X,Y,Z座標読み込み
			NVector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//座標データに追加
			positions.emplace_back(position);
		}

		//先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			//U,V成分読み込み
			NVector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向転換
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}

		//先頭文字列がvnなら法線ベクトル
		if (key == "vn")
		{
			//X,Y,Z成分読み込み
			NVector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//テクスチャ座標データに追加
			normals.emplace_back(normal);
		}

		//先頭文字列がfならポリゴン
		if (key == "f")
		{
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//頂点インデックス1行分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);	//スラッシュを飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);	//スラッシュを飛ばす
				index_stream >> indexNormal;

				//頂点データに追加
				NVertexPNU vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);

				//インデックスデータの追加
				indices.emplace_back((unsigned short)indices.size());
			}
		}
	}
	//ファイル閉じる
	file.close();
}