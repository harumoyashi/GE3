#include "NInput.h"
#include <cassert>

//xinput.lib をインポート
#pragma comment (lib,"xinput.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// DirectInputの初期化
ComPtr<IDirectInputDevice8> NInput::keyboard;
ComPtr<IDirectInput8> NInput::directInput;

// 全キーの入力状態を取得する
static BYTE keys[256] = {};

// 全キーの1F前の入力状態を取得する
static BYTE prev[256] = {};

NInput* NInput::GetInstance()
{
	static NInput instance;
	return &instance;
}

void NInput::KeyInit(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

	// DirectInputの初期化
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		//DISCL_FOREGROUND：画面が手前にある場合のみ入力を受け付ける
		//DISCL_NONEXCLUSIVE：デバイスをこのアプリだけで専有しない
		//DISCL_NOWINKEY：Windowsキーを無効にする
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void NInput::KeyUpdate()
{
	//1F前のキー情報に現在のキー情報を入れてから↓
	memcpy(prev, keys, sizeof(keys));

	//現在のキー情報の取得開始
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keys), keys);
}

//押しっぱなし
bool NInput::IsKey(UINT8 key)
{
	return keys[key];
}

//押した瞬間
bool NInput::IsKeyDown(UINT8 key)
{
	return keys[key] && !prev[key];
}

//離した瞬間
bool NInput::IsKeyRelease(UINT8 key)
{
	return !keys[key] && prev[key];
}

void NInput::PadInit()
{
	ZeroMemory(&statePad, sizeof(XINPUT_STATE));

	DWORD result;
	result = XInputGetState(0, &statePad);

	if (result == ERROR_SUCCESS)
	{
		isConnect = true;
	}
	else
	{
		isConnect = false;
	}
}

void NInput::PadUpdate()
{
	prevPad = statePad;
	DWORD result;
	result = XInputGetState(0, &statePad);
	if (result == ERROR_SUCCESS)
	{
		isConnect = true;
	}
	else
	{
		isConnect = false;
	}
	SetDeadZone();
}

bool NInput::IsButton(UINT button)
{
	return statePad.Gamepad.wButtons == button;
}

bool NInput::IsButtonDown(UINT button)
{
	return statePad.Gamepad.wButtons == button && prevPad.Gamepad.wButtons != button;
}

bool NInput::IsButtonRelease(UINT button)
{
	return statePad.Gamepad.wButtons != button && prevPad.Gamepad.wButtons == button;
}

int NInput::GetTrigger(bool isLeft)
{
	if (isLeft)
	{
		return prevPad.Gamepad.bLeftTrigger < 128 && statePad.Gamepad.bLeftTrigger >= 128;
	}
	else
	{
		return prevPad.Gamepad.bRightTrigger < 128 && statePad.Gamepad.bRightTrigger >= 128;
	}
}

void NInput::SetDeadZone()
{
	if ((statePad.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		statePad.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(statePad.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			statePad.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		statePad.Gamepad.sThumbLX = 0;
		statePad.Gamepad.sThumbLY = 0;
	}

	if ((statePad.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		statePad.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(statePad.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			statePad.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		statePad.Gamepad.sThumbRX = 0;
		statePad.Gamepad.sThumbRY = 0;
	}
}

NVector2 NInput::GetStick(bool isLeft)
{
	if (isLeft)
	{
		SHORT x = statePad.Gamepad.sThumbLX;
		SHORT y = statePad.Gamepad.sThumbLY;

		return NVector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
	}
	else
	{
		SHORT x = statePad.Gamepad.sThumbRX;
		SHORT y = statePad.Gamepad.sThumbRY;

		return NVector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
	}
}

bool NInput::IsStickUp(bool isLeft)
{
	if (isLeft)
	{
		return prevPad.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE&&
			statePad.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	}
	else
	{
		return prevPad.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE&&
			statePad.Gamepad.sThumbRY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	}
}

bool NInput::IsStickDown(bool isLeft)
{
	if (isLeft)
	{
		return prevPad.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			statePad.Gamepad.sThumbLY <= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	}
	else
	{
		return prevPad.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			statePad.Gamepad.sThumbRY <= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	}
}
