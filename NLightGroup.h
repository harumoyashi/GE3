#pragma once
#include "NDirectionalLight.h"
#include "NPointLight.h"
#include "NSpotLight.h"
#include "NCircleShadow.h"
#include "NVector3.h"
#include "NVector2.h"

class NLightGroup
{
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static const int DirLightNum = 3;
	static const int PointLightNum = 3;
	static const int SpotLightNum = 3;
	static const int CircleShadowNum = 1;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		NVector3 ambientcolor;	//環境光の色
		float pad1;	//パディング
		NDirectionalLight::ConstBufferData dirLights[DirLightNum];	//平行光源用
		NPointLight::ConstBufferData pointLights[PointLightNum];		//点光源用
		NSpotLight::ConstBufferData spotLights[SpotLightNum];		//スポットライト用
		NCircleShadow::ConstBufferData circleShadows[CircleShadowNum];		//丸影用
	};

private://静的メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;

	NVector3 ambientcolor = { 1,1,1 };				//環境光の色
	NDirectionalLight dirLights[DirLightNum];		//平行光源の配列
	NPointLight pointLights[PointLightNum];			//点光源の配列
	NSpotLight spotLights[SpotLightNum];				//点光源の配列
	NCircleShadow circleShadows[CircleShadowNum];	//丸影の配列

	//ダーティフラグ
	bool isDirty = false;

public://静的メンバ関数
	void Init();
	void Update();
	void Draw(UINT rootParamerterIndex);

	static std::unique_ptr<NLightGroup> Create();

	//定数バッファ転送
	void TransferConstBuffer();
	//環境光のライト色をセット
	void SetAmbientColor(const NVector3& color);
	//平行光源の有効フラグをセット
	void SetDirLightActive(int index, bool active);
	//平行光源のライト方向をセット
	void SetDirLightDir(int index, const NVector3& lightdir);
	//平行光源のライト色をセット
	void SetDirLightColor(int index, const NVector3& lightColor);
	//標準のライト設定
	void DefaultLightSetting();

	//点光源の座標をセット
	void SetPointLightPos(int index, const NVector3& lightpos);
	//点光源の色をセット
	void SetPointLightColor(int index, const NVector3& lightcolor);
	//点光源の減衰係数をセット
	void SetPointLightAtten(int index, const NVector3& lightatten);
	//有効フラグをセット
	void SetPointLightActive(int index, bool isActive);

	//スポットライトのライト方向をセット
	void SetSpotLightDir(int index, const NVector3& lightdir);
	//スポットライトの座標をセット
	void SetSpotLightPos(int index, const NVector3& lightpos);
	//スポットライトの色をセット
	void SetSpotLightColor(int index, const NVector3& lightcolor);
	//スポットライトの減衰係数をセット
	void SetSpotLightAtten(int index, const NVector3& lightatten);
	//スポットライトの減衰角度をセット
	void SetSpotLightFactorAngle(int index, const NVector2& lightFactorAngle);
	//有効フラグをセット
	void SetSpotLightActive(int index, bool isActive);

	//丸影のライト方向をセット
	void SetCircleShadowDir(int index, const NVector3& dir);
	//丸影の座標をセット
	void SetCircleShadowCasterPos(int index, const NVector3& casterPos);
	//丸影の座標をセット
	void SetCircleShadowDistanceCasterLight(int index, const float& distanceCasterLight);
	//丸影の減衰係数をセット
	void SetCircleShadowAtten(int index, const NVector3& atten);
	//丸影の減衰角度をセット
	void SetCircleShadowFactorAngle(int index, const NVector2& factorAngle);
	//有効フラグをセット
	void SetCircleShadowActive(int index, bool isActive);
};