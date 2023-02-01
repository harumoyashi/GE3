#pragma once
#include "NDX12.h"
#include <array>
#include <map>

#include <wrl.h>

typedef std::string TextureHandle;

class NTexture
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Resource> texBuff;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(CPU側)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(GPU側)
	std::string fileName; //ファイル名

	NTexture();
	~NTexture();
};

class NTextureManager
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	D3D12_HEAP_PROPERTIES texHeapProp{};	//ヒープ
	D3D12_RESOURCE_DESC texResDesc{};		//リソース
	static const size_t maxSRVCount = 2056;	//SRVの最大個数
	ScratchImage mipChain{};				//ミップマップ

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体

	std::map<TextureHandle, NTexture> textureMap;	//テクスチャ群
	ComPtr<ID3D12Resource> tb;	//転送用のテクスチャバッファ

public:
	//TextureManagerを取得する
	static NTextureManager* GetInstance();

	//初期化
	void Init();

	//エラーテクスチャを生成する
	NTexture CreateErrorTexture();

	//テクスチャの読み込み
	//indexを入れなかったら0番目に格納
	//読み込むたびに書き変わるから、一回きりの時だけにして
	NTexture LoadTexture(const std::string pictureName, const std::string handle = "");

private:
	//WICテクスチャのロード
	bool Load(const std::string pictureName);
	//ミップマップ生成
	void CreateMipmap();

	//ヒープ設定
	void SetTBHeap();
	//リソース設定
	void SetTBResource();
	//テクスチャバッファの生成
	ID3D12Resource* CreateTexBuff();
	//ミップマップデータの転送
	void TexBuffMaping(ID3D12Resource* texBuff);
	//シェーダーリソースビュー設定
	void SetSRV();
	//シェーダーリソースビュー生成
	NTexture CreateSRV(NTexture tex);
};

