#include "NDX12.h"
#include "NResultScene.h"
#include "NSceneManager.h"

NResultScene* NResultScene::GetInstance()
{
	static NResultScene instance;
	return &instance;
}

void NResultScene::Init()
{
#pragma region	オーディオ初期化
	audio = NAudio::GetInstance();
#pragma endregion
#pragma region	カメラ初期化
	camera.ProjectiveProjection();
	camera.CreateMatView();
	NCamera::nowCamera = &camera;
#pragma endregion
#pragma region 描画初期化処理
	//マテリアル(定数バッファ)

	//立方体情報

	//モデル情報
	for (int i = 0; i < maxModel; i++)
	{
		model[i] = std::make_unique<NModel>();
	}
	model[0]->Create("sphere");
	model[1]->Create("Cube");
	model[2]->Create("busterSword");

	//オブジェクト
	for (int i = 0; i < maxObj; i++)
	{
		obj[i] = std::make_unique<NObj3d>();
		obj[i]->Init();
	}
	obj[0]->SetModel(model[0].get());
	obj[1]->SetModel(model[1].get());
	obj[2]->SetModel(model[0].get());
	obj[3]->SetModel(model[2].get());

#pragma region オブジェクトの初期値設定
	obj[0]->position = { 0,0,0 };
	obj[1]->position = { 0,-2,0 };
	obj[1]->scale = { 10,0.1f,10 };
	obj[2]->position = { 2,0,0 };
	obj[3]->position = { -2,3,0 };
	//設定したのを適用
	for (int i = 0; i < maxObj; i++)
	{
		obj[i]->UpdateMatrix();
	}

#pragma endregion
	//背景スプライト生成

	//前景スプライト生成

#pragma endregion
	// ライト生成
	lightGroup = std::make_unique<NLightGroup>();
	lightGroup = lightGroup->Create();
	// 3Dオブジェクトにライトをセット
	NObj3d::SetLightGroup(lightGroup.get());

	lightGroup->SetDirLightActive(0, false);
	lightGroup->SetDirLightActive(1, false);
	lightGroup->SetDirLightActive(2, false);

	lightGroup->SetSpotLightActive(0, true);
	lightGroup->SetSpotLightActive(1, false);
	lightGroup->SetSpotLightActive(2, false);

	lightGroup->SetCircleShadowActive(0, false);
}

void NResultScene::Update()
{
	if (NInput::IsKeyDown(DIK_SPACE) || NInput::GetInstance()->IsButtonDown(XINPUT_GAMEPAD_A))
	{
		NSceneManager::SetScene(TITLESCENE);
	}
#pragma region 行列の計算
	//ビュー行列の再生成
	camera.CreateMatView();
	NCamera::nowCamera = &camera;

	obj[0]->MoveKey();
	obj[3]->MoveKey();

	for (size_t i = 0; i < maxObj; i++)
	{
		obj[i]->UpdateMatrix();
	}
#pragma endregion
}

void NResultScene::Draw()
{
#pragma region グラフィックスコマンド
	//背景スプライト

	//3Dオブジェクト
	for (int i = 0; i < maxObj; i++)
	{
		obj[i]->CommonBeginDraw();
		obj[i]->Draw();
	}

	//前景スプライト

	// 4.描画コマンドここまで
#pragma endregion
}

void NResultScene::Reset()
{
	// 3Dオブジェクトにライトをセット
	NObj3d::SetLightGroup(lightGroup.get());

	lightGroup->SetDirLightActive(0, false);
	lightGroup->SetDirLightActive(1, false);
	lightGroup->SetDirLightActive(2, false);

	lightGroup->SetSpotLightActive(0, true);
	lightGroup->SetSpotLightActive(1, false);
	lightGroup->SetSpotLightActive(2, false);

	lightGroup->SetCircleShadowActive(0, false);
}

void NResultScene::Finalize()
{
}