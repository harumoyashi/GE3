#include "NDX12.h"
#include "NTitleScene.h"
#include "NSceneManager.h"
#include "NInput.h"
#include "NQuaternion.h"
#include "NMathUtil.h"

NTitleScene* NTitleScene::GetInstance()
{
	static NTitleScene instance;
	return &instance;
}

void NTitleScene::Init()
{
#pragma region	オーディオ初期化
	audio = NAudio::GetInstance();
	audio->Init();
	//soundData[0] = audio->LoadWave("clear_BGM.wav");
	//audio->PlayWave(soundData[0],true);
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

	//オブジェクト
	for (int i = 0; i < maxObj; i++)
	{
		obj[i] = std::make_unique<NObj3d>();
		obj[i]->Init();
	}
	obj[0]->SetModel(model[0].get());
	obj[1]->SetModel(model[0].get());
	obj[2]->SetModel(model[1].get());

#pragma region オブジェクトの初期値設定
	obj[0]->position = { 0,0,0 };
	obj[1]->position = { 2,0,0 };
	obj[2]->position = { 0,0,0 };
	obj[2]->scale = { 10,0.1f,10 };

	//設定したのを適用
	for (int i = 0; i < maxObj; i++)
	{
		obj[i]->UpdateMatrix();
	}

	sphere.pos = obj[0]->position;
	sphere.radius = obj[0]->scale.x;
	plane.normal = { 0,1,0 };
	plane.distance = obj[2]->position.Length();
#pragma endregion
	//背景スプライト生成

	//前景スプライト生成
	foreSprite[0] = std::make_unique<NSprite>();
	foreSprite[0]->CreateSprite("hamu");

#pragma endregion
	// ライト生成
	lightGroup = std::make_unique<NLightGroup>();
	lightGroup = lightGroup->Create();
	// 3Dオブジェクトにライトをセット
	NObj3d::SetLightGroup(lightGroup.get());

	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightActive(1, true);
	lightGroup->SetDirLightActive(2, true);

	lightGroup->SetPointLightActive(0, false);
	lightGroup->SetPointLightActive(1, false);
	lightGroup->SetPointLightActive(2, false);

	lightGroup->SetCircleShadowActive(0, true);
}

void NTitleScene::Update()
{
	if (NInput::IsKeyDown(DIK_SPACE) || NInput::GetInstance()->IsButtonDown(XINPUT_GAMEPAD_A))
	{
		NSceneManager::SetScene(GAMESCENE);
	}

	lightGroup->Update();
#pragma region 行列の計算
	//ビュー行列の再生成
	camera.CreateMatView();
	NCamera::nowCamera = &camera;

	if (timer < 10000)
	{
		timer += 0.02f;
	}
	else
	{
		timer = 0;
	}
	obj[0]->position.y = sinf(timer) * 2.0f;
	if (NInput::IsKey(DIK_UP)) { obj[2]->position.y += 0.5f; }
	else if (NInput::IsKey(DIK_DOWN)) { obj[2]->position.y -= 0.5f; }
	if (NInput::IsKey(DIK_RIGHT)) { obj[2]->position.x += 0.5f; }
	else if (NInput::IsKey(DIK_LEFT)) { obj[2]->position.x -= 0.5f; }

	if (isCol)
	{
		obj[0]->model->material.SetColor(255, 0, 0, 255);
	}
	else
	{
		obj[0]->model->material.SetColor(255, 255, 255, 255);
	}
	obj[2]->model->material.SetColor(255, 255, 255, 255);

	sphere.pos = obj[0]->position;
	NVector3 vec;
	plane.distance = obj[2]->position.Dot(plane.normal);

	for (size_t i = 0; i < maxObj; i++)
	{
		obj[i]->UpdateMatrix();
	}

	isCol = NCollision::Sphere2PlaneCol(sphere, plane);
#pragma endregion
}

void NTitleScene::Draw()
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
	if (isCol)
	{
		foreSprite[0]->CommonBeginDraw();
		foreSprite[0]->Draw();
	}

	// 4.描画コマンドここまで
#pragma endregion
}

void NTitleScene::Reset()
{
	// 3Dオブジェクトにライトをセット
	NObj3d::SetLightGroup(lightGroup.get());

	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightActive(1, true);
	lightGroup->SetDirLightActive(2, true);

	lightGroup->SetPointLightActive(0, false);
	lightGroup->SetPointLightActive(1, false);
	lightGroup->SetPointLightActive(2, false);

	lightGroup->SetCircleShadowActive(0, true);
}

void NTitleScene::Finalize()
{
}