#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <assert.h>
#include <wrl.h>
#include "NVector3.h"

class NPointLight
{
public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		NVector3 lightPos;
		float pad1;	//パディング
		NVector3 lightcolor;	//ライトの色
		float pad2;	//パディング
		NVector3 lightatten;
		unsigned int isActive = false;	//有効フラグ
	};

private://静的メンバ変数
	NVector3 lightpos = { 0,0,0 };		// ライト座標(ワールド)
	NVector3 lightcolor = { 1,1,1 };	// ライト色
	NVector3 lightatten = { 0.2f,0.2f,0.2f };	// ライト距離減衰係数

	//有効フラグ
	bool isActive = false;

public://メンバ関数
	//ライトの座標をセット
	inline void SetLightPos(const NVector3& lightpos) { this->lightpos = lightpos; }
	inline NVector3 GetLightPos() { return lightpos; }
	//ライトの色をセット
	inline void SetLightColor(const NVector3& lightcolor) { this->lightcolor = lightcolor; }
	inline NVector3 GetLightColor() { return lightcolor; }
	//ライトの減衰係数をセット
	inline void SetLightAtten(const NVector3& lightatten) { this->lightatten = lightatten; }
	inline NVector3 GetLightAtten() { return lightatten; }
	//有効フラグをセット
	inline void SetActive(bool isActive) { this->isActive = isActive; }
	//有効フラグを取得
	inline bool GetActive() { return isActive; }
};