#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <assert.h>
#include <wrl.h>
#include "NVector3.h"
#include "NVector2.h"
#include "NMathUtil.h"

class NSpotLight
{
public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		NVector3 lightv;
		float pad1;	//パディング
		NVector3 lightPos;
		float pad2;	//パディング
		NVector3 lightcolor;	//ライトの色
		float pad3;	//パディング
		NVector3 lightatten;
		float pad4;	//パディング
		NVector2 lightfactoranglecos;
		unsigned int isActive = false;	//有効フラグ
		float pad5;	//パディング
	};

private://静的メンバ変数
	NVector3 lightdir = { 1,0,0 };	// ライト方向
	NVector3 lightpos = { 0,0,0 };		// ライト座標(ワールド)
	NVector3 lightcolor = { 1,1,1 };	// ライト色
	NVector3 lightatten = { 1,1,1 };	// ライト距離減衰係数
	NVector2 lightFactorAngleCos = { 0.5f,0.2f };	// ライト減衰角度

	//有効フラグ
	bool isActive = false;

public://メンバ関数
	inline void SetLightDir(const NVector3& lightdir) { this->lightdir = lightdir.Normalize(); }
	inline const NVector3& GetLightDir() { return lightdir; }
	//ライトの座標をセット
	inline void SetLightPos(const NVector3& lightpos) { this->lightpos = lightpos; }
	inline const NVector3& GetLightPos() { return lightpos; }
	//ライトの色をセット
	inline void SetLightColor(const NVector3& lightcolor) { this->lightcolor = lightcolor; }
	inline const NVector3& GetLightColor() { return lightcolor; }
	//ライトの減衰係数をセット
	inline void SetLightAtten(const NVector3& lightatten) { this->lightatten = lightatten; }
	inline const NVector3& GetLightAtten() { return lightatten; }
	//ライトの減衰角度をセット
	inline void SetLightFactorAngle(const NVector2& lightFactorAngle) {
		this->lightFactorAngleCos.x = cosf(MathUtil::Degree2Radian(lightFactorAngle.x));
		this->lightFactorAngleCos.y = cosf(MathUtil::Degree2Radian(lightFactorAngle.y));
	}
	inline const NVector2& GetLightFactorAngle() { return lightFactorAngleCos; }
	//有効フラグをセット
	inline void SetActive(bool isActive) { this->isActive = isActive; }
	//有効フラグを取得
	inline bool GetActive() { return isActive; }
};