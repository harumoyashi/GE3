#pragma once
#include <DirectXMath.h>
#include <memory>

#include "NVector2.h"
#include "NVector3.h"
#include "NMatrix4.h"

using namespace DirectX;

class NCamera
{
public:
	static std::unique_ptr<NCamera> _nCamera;
	static NCamera* nowCamera;

private:
	//カメラ
	NMatrix4 matProjection{};
	NMatrix4 matView{};

	//カメラ
	NVector3 eye = { 0, 10.0f, -30.0f };	//視点座標
	NVector3 target = { 0, 0, 0 };	//注視点座標
	NVector3 up = { 0, 1.0f, 0 };		//上方向ベクトル
	float angle = 0.0f;				//カメラの回転角

public:
	//ビュー変換行列作成
	void CreateMatView();
	//射影投影変換//
	void ProjectiveProjection();

	//セッター
	void SetEye(NVector3 eye) { this->eye = eye; }
	void SetTarget(NVector3 target) { this->target = target; }
	void SetUp(NVector3 up) { this->up = up; }

	//ゲッター
	NMatrix4 GetMatProjection() { return matProjection; }
	NMatrix4 GetMatView() { return matView; }
	//eye座標取得
	NVector3 GetPos() { return eye; }
};