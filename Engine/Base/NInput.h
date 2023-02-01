#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#include "NVector2.h"

#include <wrl.h>

class NInput
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static ComPtr<IDirectInputDevice8> keyboard;
	static ComPtr<IDirectInput8> directInput;

public:
	NInput() {};
	static NInput* GetInstance();

	//input初期化
	static void KeyInit(HINSTANCE hInstance, HWND hwnd);
	//input更新
	static void KeyUpdate();

	//キーボード入力処理用 (返り値0,1)
	//押しっぱなし
	static bool IsKey(UINT8 key);	//UINTはビット数指定したら型自由ぽい
	//押した瞬間
	static bool IsKeyDown(UINT8 key);
	//離した瞬間
	static bool IsKeyRelease(UINT8 key);

public:
	//XINPUT_STATE 構造体のインスタンスを作成
	XINPUT_STATE statePad{};
	XINPUT_STATE prevPad{};

	//接続されてるか
	bool isConnect = false;

public:
	//初期化
	void PadInit();
	//更新
	void PadUpdate();

	//押しっぱなし
	bool IsButton(UINT button);    //UINTはビット数指定したら型自由ぽい
	//押した瞬間
	bool IsButtonDown(UINT button);
	//離した瞬間
	bool IsButtonRelease(UINT button);

	//トリガーの押し込み具合取得
	//isLeft:右左どっち！
	int GetTrigger(bool isLeft = true);

	//デッドゾーンの設定
	void SetDeadZone();

	//スティックの傾き具合取得
	//isLeft:右左どっち！
	NVector2 GetStick(bool isLeft = true);

	//スティックを上に倒した瞬間か
	//isLeft:右左どっち！
	bool IsStickUp(bool isLeft = true);
	//スティックを下に倒した瞬間か
	//isLeft:右左どっち！
	bool IsStickDown(bool isLeft = true);
};