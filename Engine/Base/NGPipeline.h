#pragma once
#include <DirectXMath.h>
#include "NDX12.h"
#include "NTexture.h"
#include "NRootSignature.h"
#include "NRootParam.h"
#include <d3dx12.h>

#include <wrl.h>

template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct PipelineSet
{
	ComPtr<ID3D12PipelineState> pipelineState;
	NRootSignature rootSig;
};

class NGPipeline
{
private:
	//シェーダーリソースビュー//
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体

	//シェーダーまわり(頂点も含む)//
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	//頂点レイアウト//
	D3D12_INPUT_ELEMENT_DESC vertLayout3d[3]{};		//必要な分だけ用意する
	D3D12_INPUT_ELEMENT_DESC vertLayoutSprite[2]{};	//必要な分だけ用意する

	//パイプラインステート//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc = {};

	//テクスチャサンプラー//
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};

	NRootParam rootParams;	//ルートパラメータ

public:
	//参照するものたち
	PipelineSet pipelineSet;			//パイプラインステートとルートシグネチャまとめたやつ

public:
#pragma region シェーダーまわり
	//3Dオブジェクト用頂点シェーダーの読み込みとコンパイル
	void LoadVertShader3d();
	//3Dオブジェクト用ピクセルシェーダの読み込みとコンパイル
	void LoadPixelShader3d();
	//スプライト用頂点シェーダーの読み込みとコンパイル
	void LoadVertShaderSprite();
	//スプライト用ピクセルシェーダの読み込みとコンパイル
	void LoadPixelShaderSprite();
#pragma endregion
#pragma region 頂点レイアウトまわり
	//3Dオブジェクト用頂点レイアウト設定
	void SetVertLayout3d();
	//スプライト用頂点レイアウト設定
	void SetVertLayoutSprite();
#pragma endregion
#pragma region パイプラインステートまわり
	//シェーダーの設定(適用)
	void SetShader();
	//ラスタライザー設定
	void SetRasterizer(bool isCull);
	//ブレンド設定
	void SetBlend();
	//入力レイアウトの設定
	void SetInputLayout(bool is3d);
	//図形の形状設定
	void SetTopology();
	//デプスステンシル(深度)設定
	void SetDepth(bool isDepth);
	//レンダーターゲット設定
	void SetRenderTarget();
	//アンチエイリアシングのためのサンプル数設定
	void SetAntiAliasing();
	//パイプラインにルートシグネチャをセット
	void SetRootSignature();
	//グラフィックスパイプラインステートオブジェクトの生成
	void CreatePS();
#pragma endregion
#pragma region テクスチャサンプラー
	//テクスチャサンプラー設定
	void SetTexSampler();
#pragma endregion
#pragma region パイプライン生成
	//3Dオブジェクト用パイプライン生成
	PipelineSet CreatePipeline3d();
	//スプライト用パイプライン生成
	PipelineSet CreatePipelineSprite();
#pragma endregion

private:

};


class PipeLineManager
{
private:
	NGPipeline pipeline3d;
	NGPipeline pipelineSprite;

	PipelineSet pipelineSet3d;
	PipelineSet pipelineSetSprite;

public:
	PipeLineManager();
	~PipeLineManager();

	static PipeLineManager* GetInstance();
	void Init();

	PipelineSet GetPipelineSet3d() const { return pipelineSet3d; }
	PipelineSet GetPipelineSetSprite() const { return pipelineSetSprite; }
};
