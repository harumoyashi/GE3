#pragma once
#include "NDX12.h"
#include "NTexture.h"

#include "NVector2.h"
#include "NVector3.h"

#include<memory>
#include <wrl.h>
template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//定数バッファ用構造体
struct ConstBufferDataMaterial
{
	NVector3 ambient;	//アンビエント係数
	float pad1;			//パディング(隙間埋め)
	NVector3 diffuse;	//ディフューズ係数
	float pad2;			//パディング
	NVector3 specular;	//スペキュラー係数
	float alpha;		//α値
};

class NMaterial
{
private:
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ
	D3D12_RESOURCE_DESC resDesc{};		//リソース

public:
	ComPtr<ID3D12Resource> constBuffMaterial;
	ConstBufferDataMaterial* constMapMaterial;		//色(RGBA)

	std::string name;	//マテリアル
	NVector3 ambient;	//アンビエント影響度
	NVector3 diffuse;	//ディフューズ影響度
	NVector3 specular;	//スペキュラー影響度
	float alpha;		//α値
	std::string textureFilename;	//テクスチャファイル名
	NTexture texture;

public:
	//コンストラクタ
	NMaterial();
	~NMaterial();
#pragma region 初期化まわり
	//初期化
	void Init();
	//ヒープ設定
	void SetHeap();
	//リソース設定
	void SetResource();
	//定数バッファの生成
	void CreateCB();
	//定数バッファのマッピング
	void MappingCB();
	//光情報転送
	void TransferLight();
	//色情報転送
	void TransferColor();
	//色情報変更(int型0~255)
	void SetColor(int R = 255, int G = 255, int B = 255, int A = 255);
#pragma endregion
#pragma region 描画まわり
	//定数バッファビュー(CBV)の設定コマンド
	void SetCBV();
#pragma endregion
};

