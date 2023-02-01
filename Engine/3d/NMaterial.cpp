#include "NMaterial.h"


NMaterial::NMaterial()
{
	Init();
	//�Ƃ肠�����`�悳���悤�ɐݒ肵�Ƃ�
	ambient = { 0.3f,0.3f,0.3f };
	diffuse = { 0.0f,0.0f,0.0f };
	specular = { 0.0f,0.0f,0.0f };
	alpha = 1.0f;
}

NMaterial::~NMaterial()
{
	//constMapTransform.reset();
	//constMapMaterial.reset();
}

void NMaterial::Init()
{
	SetHeap();
	SetResource();
	CreateCB();
	MappingCB();
	TransferColor();
	TransferLight();
}

void NMaterial::SetHeap()
{
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
}

void NMaterial::SetResource()
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void NMaterial::CreateCB()
{
	HRESULT result;

	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,		//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));
}

void NMaterial::MappingCB()
{
	HRESULT result;

	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//�}�b�s���O
	assert(SUCCEEDED(result));
}

void NMaterial::TransferLight()
{
	constMapMaterial->ambient = ambient;
	constMapMaterial->diffuse = diffuse;
	constMapMaterial->specular = specular;
}

void NMaterial::TransferColor()
{
	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial->alpha = 1.0f;
}

void NMaterial::SetColor(int R, int G, int B, int A)
{
	XMFLOAT4 color = {};

	color.x = static_cast<float>(R) / 255.0f;
	color.y = static_cast<float>(G) / 255.0f;
	color.z = static_cast<float>(B) / 255.0f;
	alpha = static_cast<float>(A) / 255.0f;
	constMapMaterial->alpha = alpha;
}

void NMaterial::SetCBV()
{
	NDX12::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
}
