#pragma once
#include <vector>
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
#include "NAudio.h"
#include "NCamera.h"
#include "NCollision.h"
#include "NLightGroup.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class NResultScene
{
private:
	//オブジェクト
	NMaterial material;				//マテリアル
	static const int maxObj = 3;	//オブジェクト数
	std::array<std::unique_ptr<NObj3d>, maxObj> obj;			//オブジェクト(定数バッファ)

	static const int maxModel = 2;	//モデル数
	std::array<std::unique_ptr<NModel>, maxModel> model;	//モデル情報

	NCollision::Sphere sphere;
	NCollision::Plane plane;
	bool isCol = false;

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
	//インスタンス取得
	static NResultScene* GetInstance();

	void Init();
	void Update();
	void Draw();
	void Reset();
	void Finalize();
};