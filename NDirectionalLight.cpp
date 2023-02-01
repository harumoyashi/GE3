#include "NDirectionalLight.h"
#include "NDX12.h"

NDirectionalLight* NDirectionalLight::Creare()
{
	//3Dオブジェクトのインスタンスを生成
	NDirectionalLight* instance = new NDirectionalLight();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
	return instance;
}

void NDirectionalLight::Initialize()
{
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

void NDirectionalLight::Update()
{
	//値の更新があったときだけ定数バッファに転送する
	if (isDirty) {
		TransferConstBuffer();
		isDirty = false;
	}
}

void NDirectionalLight::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex,
		constBuff->GetGPUVirtualAddress());
}

void NDirectionalLight::TransferConstBuffer()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->lightv = -lightdir;	//ライトの向きは逆向きで
		constMap->lightcolor = lightcolor;
		constBuff->Unmap(0, nullptr);
	}
}

void NDirectionalLight::SetLightDir(const NVector3& lightdir)
{
	//正規化してセット
	this->lightdir = lightdir.Normalize();
	isDirty = true;
}

void NDirectionalLight::SetLightColor(const NVector3& lightcolor)
{
	this->lightcolor = lightcolor;
	isDirty = true;
}