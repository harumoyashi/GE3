#pragma once
#include "NVector3.h"

class NMatrix4
{
public:
	float m[4][4];

	//単位行列で生成
	NMatrix4() :
		m{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 }{}
	NMatrix4(
		float _00, float _01, float _02, float _03,
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33):
		m {
			_00, _01, _02, _03,
			_10, _11, _12, _13,
			_20, _21, _22, _23,
			_30, _31, _32, _33}{}

	//逆行列
	NMatrix4 operator-() const;

	//代入演算子オーバーロード
	//加算
	NMatrix4 operator+(const NMatrix4& m) const;
	//加算
	NMatrix4& operator+=(const NMatrix4& m);
	//減算
	NMatrix4 operator-(const NMatrix4& m) const;
	//減算
	NMatrix4& operator-=(const NMatrix4& m);
	//乗算
	NMatrix4 operator*(const NMatrix4& m) const;
	//乗算
	NMatrix4& operator*=(const NMatrix4& m);

	//全ての要素が0の行列
	NMatrix4 Zero();
	//単位行列
	NMatrix4 Identity();
	//拡縮行列の設定
	NMatrix4 Scale(const NVector3& s);
	//回転行列の設定
	NMatrix4 RotateX(float angle);
	NMatrix4 RotateY(float angle);
	NMatrix4 RotateZ(float angle);
	//平行移動行列の作成
	NMatrix4 Translation(const NVector3& t);
	//座標変換（ベクトルと行列の掛け算）
	NVector3 Transform(const NVector3& v, const NMatrix4& m);
	//w除算を使った座標変換
	NVector3 WTransform(const NVector3& v, const NMatrix4& m);

	//オイラー角への変換
	//参考:https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf#%E5%9B%9E%E8%BB%A2%E8%A1%8C%E5%88%97%E3%81%8B%E3%82%89%E3%82%AA%E3%82%A4%E3%83%A9%E3%83%BC%E8%A7%92
	NVector3 ToEuler() const;
};

NVector3 operator*(const NVector3 v, const NMatrix4 m);
NVector3& operator*=(NVector3& v, const NMatrix4 m);