#include "NLightGroup.h"
#include "NDX12.h"

void NLightGroup::Init()
{
	DefaultLightSetting();

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	HRESULT result;
	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	assert(SUCCEEDED(result));

	//定数バッファへデータ転送
	TransferConstBuffer();
}

void NLightGroup::Update()
{
	//値の更新があった時だけ定数バッファに転送する
	if (isDirty)
	{
		TransferConstBuffer();
		isDirty = false;
	}
}

void NLightGroup::Draw(UINT rootParamerterIndex)
{
	//定数バッファビューをセット
	NDX12::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(rootParamerterIndex, constBuff->GetGPUVirtualAddress());
}

std::unique_ptr<NLightGroup> NLightGroup::Create()
{
	//3Dオブジェクトのインスタンスを生成
	std::unique_ptr<NLightGroup> instance = std::make_unique<NLightGroup>();
	//初期化
	instance->Init();

	return instance;
}

void NLightGroup::TransferConstBuffer()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambientcolor = ambientcolor;
		for (int i = 0; i < DirLightNum; i++)
		{
			if (dirLights[i].GetActive())
			{
				constMap->dirLights[i].isActive = 1;
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightcolor = dirLights[i].GetLightColor();
			}
			else
			{
				constMap->dirLights[i].isActive = 0;
			}
		}

		for (int i = 0; i < PointLightNum; i++)
		{
			if (pointLights[i].GetActive())
			{
				constMap->pointLights[i].isActive = 1;
				constMap->pointLights[i].lightPos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor = pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights[i].GetLightAtten();
			}
			else
			{
				constMap->pointLights[i].isActive = 0;
			}
		}

		for (int i = 0; i < SpotLightNum; i++)
		{
			if (spotLights[i].GetActive())
			{
				constMap->spotLights[i].isActive = 1;
				constMap->spotLights[i].lightv = -spotLights[i].GetLightDir();
				constMap->spotLights[i].lightPos = spotLights[i].GetLightPos();
				constMap->spotLights[i].lightcolor = spotLights[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos = spotLights[i].GetLightFactorAngle();
			}
			else
			{
				constMap->spotLights[i].isActive = 0;
			}
		}

		for (int i = 0; i < CircleShadowNum; i++)
		{
			if (circleShadows[i].GetActive())
			{
				constMap->circleShadows[i].isActive = 1;
				constMap->circleShadows[i].dir = -circleShadows[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows[i].GetAtten();
				constMap->circleShadows[i].factoranglecos = circleShadows[i].GetFactorAngle();
			}
			else
			{
				constMap->circleShadows[i].isActive = 0;
			}
		}
		constBuff->Unmap(0, nullptr);
	}
}

void NLightGroup::SetAmbientColor(const NVector3& color)
{
	ambientcolor = color;
	isDirty = true;
}

void NLightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetActive(active);
}

void NLightGroup::SetDirLightDir(int index, const NVector3& lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetLightDir(lightdir);
	isDirty = true;
}

void NLightGroup::SetDirLightColor(int index, const NVector3& lightColor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetLightColor(lightColor);
	isDirty = true;
}

void NLightGroup::DefaultLightSetting()
{
	dirLights[0].SetActive(true);
	dirLights[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[0].SetLightDir({ 0.0f, -1.0f, 0.0f });

	dirLights[1].SetActive(true);
	dirLights[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[1].SetLightDir({ +0.5f, +0.1f, +0.2f });

	dirLights[2].SetActive(true);
	dirLights[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[2].SetLightDir({ -0.5f, +0.1f, -0.2f });
}

void NLightGroup::SetPointLightPos(int index, const NVector3& lightpos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightPos(lightpos);
	isDirty = true;
}

void NLightGroup::SetPointLightColor(int index, const NVector3& lightcolor)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightColor(lightcolor);
	isDirty = true;
}

void NLightGroup::SetPointLightAtten(int index, const NVector3& lightatten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightAtten(lightatten);
	isDirty = true;
}

void NLightGroup::SetPointLightActive(int index, bool isActive)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetActive(isActive);
	isDirty = true;
}

void NLightGroup::SetSpotLightDir(int index, const NVector3& lightdir)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightDir(lightdir);
	isDirty = true;
}

void NLightGroup::SetSpotLightPos(int index, const NVector3& lightpos)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightPos(lightpos);
	isDirty = true;
}

void NLightGroup::SetSpotLightColor(int index, const NVector3& lightcolor)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightColor(lightcolor);
	isDirty = true;
}

void NLightGroup::SetSpotLightAtten(int index, const NVector3& lightatten)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightAtten(lightatten);
	isDirty = true;
}

void NLightGroup::SetSpotLightFactorAngle(int index, const NVector2& lightFactorAngle)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightFactorAngle(lightFactorAngle);
	isDirty = true;
}

void NLightGroup::SetSpotLightActive(int index, bool isActive)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetActive(isActive);
	isDirty = true;
}

void NLightGroup::SetCircleShadowDir(int index, const NVector3& dir)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetDir(dir);
	isDirty = true;
}

void NLightGroup::SetCircleShadowCasterPos(int index, const NVector3& casterPos)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetCasterPos(casterPos);
	isDirty = true;
}

void NLightGroup::SetCircleShadowDistanceCasterLight(int index, const float& distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetDistanceCasterLight(distanceCasterLight);
	isDirty = true;
}

void NLightGroup::SetCircleShadowAtten(int index, const NVector3& atten)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetAtten(atten);
	isDirty = true;
}

void NLightGroup::SetCircleShadowFactorAngle(int index, const NVector2& factorAngle)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetFactorAngle(factorAngle);
	isDirty = true;
}

void NLightGroup::SetCircleShadowActive(int index, bool isActive)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetActive(isActive);
	isDirty = true;
}
