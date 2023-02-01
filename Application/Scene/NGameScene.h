#pragma once
#include <vector>
#include <algorithm>
#include <wrl.h>

#include "NRootParam.h"
#include "NMaterial.h"
#include "NGPipeline.h"
#include "NRootSignature.h"
#include "NObj3d.h"
#include "NTexture.h"
#include "NSprite.h"
#include "NPreDraw.h"
#include "NModel.h"
#include "NCamera.h"
#include "NAudio.h"
#include "NLightGroup.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class NGameScene
{
private:
	//オブジェクト
	NMaterial material;				//マテリアル
	static const int maxObj = 4;	//オブジェクト数
	std::unique_ptr<NObj3d> obj[maxObj];			//オブジェクト(定数バッファ)

	static const int maxModel = 3;	//モデル数
	std::array<std::unique_ptr<NModel>, maxModel> model;	//モデル情報

	//背景スプライト
	static const int maxBackSprite = 3;	//背景スプライト数
	std::unique_ptr<NSprite> backSprite[maxBackSprite];	//背景スプライト

	//前景スプライト
	static const int maxForeSprite = 3;	//前景スプライト数
	std::unique_ptr<NSprite> foreSprite[maxForeSprite];	//前景スプライト

	NCamera camera;	//カメラ

	//オーディオ
	NAudio* audio = nullptr;
	static const int maxSoundData = 3;		//サウンドデータの最大数
	uint32_t soundData[maxSoundData] = {};	//サウンドデータ格納用

	std::unique_ptr<NLightGroup> lightGroup;

public:


public:
	//インスタンス取得
	static NGameScene* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();
	void Reset();
	//終了処理
	void Finalize();

private:


};