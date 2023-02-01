#pragma once
#include "NTitleScene.h"
#include "NGameScene.h"

enum Scene {
	TITLESCENE,
	GAMESCENE,
};

class NSceneManager
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	NTitleScene* titleScene;
	NGameScene* gameScene;	//ゲームシーン

	static int scene;		//現在のシーン
	static bool isSceneChange;	//シーンの初期化フラグ

public:


public:
	//インスタンス取得
	static NSceneManager* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();
	//終了処理
	void Finalize();
	//シーンの変更
	static void SetScene(int selectScene);

	NSceneManager();

#pragma region ゲッター
#pragma endregion
private:


};