#include "NWindows.h"
#include "NDX12.h"
#include "NGPipeline.h"
#include "NAudio.h"
#include "NTexture.h"
#include "NSceneManager.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region WindowsAPI初期化
	NWindows* win = NWindows::GetInstance();
	win->Set();
	win->CreateWindowObj();
	win->Display();
#pragma endregion
#pragma region DirectX初期化
	NDX12* dx12 = NDX12::GetInstance();
	dx12->Init(win);
	//input初期化
	NInput::KeyInit(win->GetHInstance(), win->GetHwnd());
	NInput::GetInstance()->PadInit();
	//グラフィックスパイプライン初期化
	PipeLineManager* pipeline = PipeLineManager::GetInstance();
	pipeline->Init();
#pragma endregion
#pragma region オーディオ初期化
	NAudio* audio = NAudio::GetInstance();
	audio->Init();
#pragma endregion
#pragma region テクスチャマネージャー初期化
	NTextureManager* texManager = NTextureManager::GetInstance();
	texManager->Init();
#pragma endregion
#pragma region ゲームシーン初期化
	NSceneManager* sceneMane = NSceneManager::GetInstance();
	sceneMane->Init();
#pragma endregion
	//ゲームループ
	while (true)
	{
#pragma region ウィンドウメッセージ処理
		if (win->WindowMessage()) { break; }
#pragma endregion
#pragma region DirectX毎フレーム処理
		//DirectX毎フレーム　ここから
		NInput::KeyUpdate();
		NInput::GetInstance()->PadUpdate();
		sceneMane->Update();
		sceneMane->Draw();
	}
#pragma endregion
#pragma region WindowsAPI後始末
	audio->Finalize();
	win->Finalize();
	sceneMane->Finalize();
#pragma endregion
	return 0;
}