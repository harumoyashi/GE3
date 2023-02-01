#include "NIndexBuff.h"
#include "NDX12.h"

NIndexBuff::NIndexBuff(unsigned int* indices, unsigned int size)
{
	Init(indices, size);
}

NIndexBuff::NIndexBuff(std::vector<unsigned int> indices)
{
	Init(indices);
}

void NIndexBuff::Init(unsigned int* indices, unsigned int size)
{
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * size);

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	//インデックスバッファをマッピング
	UINT* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));
	// 全インデックスに対して
	for (UINT i = 0; i < size; i++)
	{
		indexMap[i] = indices[i];	//インデックスをコピー
	}
	// 繋がりを解除
	buff->Unmap(0, nullptr);

	// GPU仮想アドレス
	view.BufferLocation = buff->GetGPUVirtualAddress();
	//インデックス1個分のサイズ
	view.Format = DXGI_FORMAT_R32_UINT;
	// インデックスバッファのサイズ
	view.SizeInBytes = dataSize;
}

void NIndexBuff::Init(std::vector<unsigned int> indices)
{
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	UINT dataSize = static_cast<UINT>(sizeof(UINT) * indices.size());

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	//インデックスバッファをマッピング
	UINT* indexMap = nullptr;
	result = buff->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));
	// 全インデックスに対して
	for (UINT i = 0; i < indices.size(); i++)
	{
		indexMap[i] = indices[i];	//インデックスをコピー
	}
	// 繋がりを解除
	buff->Unmap(0, nullptr);

	// GPU仮想アドレス
	view.BufferLocation = buff->GetGPUVirtualAddress();
	//インデックス1個分のサイズ
	view.Format = DXGI_FORMAT_R32_UINT;
	// インデックスバッファのサイズ
	view.SizeInBytes = dataSize;
}
