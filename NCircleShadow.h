#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <assert.h>
#include <wrl.h>
#include "NVector3.h"
#include "NVector2.h"
#include "NMathUtil.h"

class NCircleShadow
{
public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		NVector3 dir;
		float pad1;	//パディング
		NVector3 casterPos;
		float distanceCasterLight;
		NVector3 atten;
		float pad2;	//パディング
		NVector2 factoranglecos;
		unsigned int isActive;	//有効フラグ
		float pad3;	//パディング
	};

private://静的メンバ変数
	NVector3 dir = { 1,0,0 };			// 方向
	float distanceCasterLight = 100.0f;	// キャスターとライトの距離
	NVector3 casterPos = { 0,0,0 };		// キャスター座標(ワールド)
	NVector3 atten = { 0.5f,0.6f,0 };			// 距離減衰係数
	NVector2 factorAngleCos = { 0.2f,0.5f };	// 減衰角度

	//有効フラグ
	bool isActive = false;

public://メンバ関数
	inline void SetDir(const NVector3& dir) { this->dir = dir.Normalize(); }
	inline const NVector3& GetDir() { return dir; }
	//座標をセット
	inline void SetCasterPos(const NVector3& casterPos) { this->casterPos = casterPos; }
	inline const NVector3& GetCasterPos() { return casterPos; }
	//キャスターとライトの距離をセット
	inline void SetDistanceCasterLight(const float& distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }
	inline const float& GetDistanceCasterLight() { return distanceCasterLight; }
	//減衰係数をセット
	inline void SetAtten(const NVector3& atten) { this->atten = atten; }
	inline const NVector3& GetAtten() { return atten; }
	//減衰角度をセット
	inline void SetFactorAngle(const NVector2& factorAngle) {
		this->factorAngleCos.x = cosf(MathUtil::Degree2Radian(factorAngle.x));
		this->factorAngleCos.y = cosf(MathUtil::Degree2Radian(factorAngle.y));
	}
	inline const NVector2& GetFactorAngle() { return factorAngleCos; }
	//有効フラグをセット
	inline void SetActive(bool isActive) { this->isActive = isActive; }
	//有効フラグを取得
	inline bool GetActive() { return isActive; }
};