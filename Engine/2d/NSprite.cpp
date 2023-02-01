#include "NSprite.h"
#include "NMathUtil.h"

NSprite::~NSprite()
{
}

void NSprite::CreateSprite(std::string texHandle)
{
	SetTexHandle(texHandle);
	//頂点
	SetVert();
	SetVertHeap();
	SetVertResource();
	CreateVertBuff();
	VertMaping();
	CreateVertBuffView();
	//定数バッファ
	SetConstHeap();
	SetConstResource();
	CreateCB();
	MappingCB();
	SetColor();
	Unmap();
	//平行投影を代入
	matProjection = constMapTransform->mat = MathUtil::ParallelProjection(
		static_cast<float>(NWindows::win_width),
		static_cast<float>(NWindows::win_height)
	);
	UpdateMatrix();
}

void NSprite::CreateSprite(std::string texHandle,
	NVector2 anchorPoint, bool isFlipX, bool isFlipY)
{
	SetTexHandle(texHandle);
	//頂点
	SetVert();
	SetVertHeap();
	SetVertResource();
	CreateVertBuff();
	MatchTexSize(NTextureManager::GetInstance()->textureMap[this->texHandle].texBuff);	//ここでテクスチャサイズに合わせてる
	SetAncor(anchorPoint);
	SetIsFlip(isFlipX, isFlipY);
	SetClipRange();
	TransferVertex();
	VertMaping();
	CreateVertBuffView();
	//定数バッファ
	SetConstHeap();
	SetConstResource();
	CreateCB();
	MappingCB();
	SetColor();
	Unmap();
	//平行投影を代入
	matProjection = constMapTransform->mat = MathUtil::ParallelProjection(
		static_cast<float>(NWindows::win_width),
		static_cast<float>(NWindows::win_height)
	);
	UpdateMatrix();
}

void NSprite::CreateClipSprite(std::string texHandle,
	NVector2 texLeftTop, NVector2 texSize, NVector2 anchorPoint, bool isFlipX, bool isFlipY)
{
	SetTexHandle(texHandle);
	//頂点
	SetVert();
	SetVertHeap();
	SetVertResource();
	CreateVertBuff();
	MatchTexSize(NTextureManager::GetInstance()->textureMap[texHandle].texBuff);	//ここでテクスチャサイズに合わせてる
	SetAncor(anchorPoint);
	SetIsFlip(isFlipX, isFlipY);
	SetClipRange(texLeftTop, texSize);
	TransferVertex();
	VertMaping();
	CreateVertBuffView();
	//定数バッファ
	SetConstHeap();
	SetConstResource();
	CreateCB();
	MappingCB();
	SetColor();
	Unmap();
	//平行投影を代入
	matProjection = constMapTransform->mat = MathUtil::ParallelProjection(
		static_cast<float>(NWindows::win_width),
		static_cast<float>(NWindows::win_height)
	);
	UpdateMatrix();
}

void NSprite::SetVert()
{
	VertexUV vert[] = {
		//	x		y		z	 	u	v
		{{   0.0f, 100.0f, 0.0f }, {0.0f,1.0f}},	// 左下
		{{   0.0f,   0.0f, 0.0f }, {0.0f,0.0f}},	// 左上
		{{ 100.0f, 100.0f, 0.0f }, {1.0f,1.0f}},	// 右下
		{{ 100.0f,   0.0f, 0.0f }, {1.0f,0.0f}},	// 右上
	};

	//設定したら他でも使える変数に代入
	std::copy(std::begin(vert), std::end(vert), vertices);

	//頂点バッファのサイズを代入
	singleSizeVB = static_cast<UINT>(sizeof(vertices[0]));
	sizeVB = static_cast<UINT>(sizeof(vertices));
}

void NSprite::SetVertHeap()
{
	//ヒープ設定
	heapPropVert.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	heapPropVert.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropVert.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
}

void NSprite::SetVertResource()
{
	//リソース設定
	resDescVert.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDescVert.Width = sizeVB;			//頂点データ全体のサイズ
	resDescVert.Height = 1;
	resDescVert.DepthOrArraySize = 1;
	resDescVert.MipLevels = 1;
	resDescVert.Format = DXGI_FORMAT_UNKNOWN;
	resDescVert.SampleDesc.Count = 1;
	resDescVert.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDescVert.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void NSprite::CreateVertBuff()
{
	HRESULT result;

	// 頂点バッファの生成
	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapPropVert, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDescVert, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));
}

void NSprite::MatchTexSize(ComPtr<ID3D12Resource> texBuff)
{
	resDescVert = texBuff->GetDesc();
	size = { (float)resDescVert.Width,(float)resDescVert.Height };
}

void NSprite::SetAncor(NVector2 anchorPoint)
{
	this->anchorPoint = anchorPoint;
}

void NSprite::SetIsFlip(bool isFlipX, bool isFlipY)
{
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
}

void NSprite::SetClipRange(NVector2 texLeftTop, NVector2 texSize)
{
	this->texLeftTop = texLeftTop;
	this->texSize = texSize;
	size = texSize;
}

void NSprite::SetClipRange()
{
	texLeftTop = { 0,0 };
	texSize = size;
}

void NSprite::VertMaping()
{
	HRESULT result;

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	std::copy(std::begin(vertices), std::end(vertices), vertMap);	//座標をコピー
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);
}

void NSprite::CreateVertBuffView()
{
	// 頂点バッファビューの作成
	// GPU仮想アドレス
	//これ渡すことで、GPU側がどのデータ見ればいいかわかるようになる
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = singleSizeVB;
}

void NSprite::SetConstHeap()
{
	heapPropConst.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
}

void NSprite::SetConstResource()
{
	resDescConst.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDescConst.Width = (sizeof(constMapTransform) + 0xff) & ~0xff;	//256バイトアラインメント
	resDescConst.Height = 1;
	resDescConst.DepthOrArraySize = 1;
	resDescConst.MipLevels = 1;
	resDescConst.SampleDesc.Count = 1;
	resDescConst.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void NSprite::CreateCB()
{
	HRESULT result;

	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapPropConst,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDescConst,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);
	assert(SUCCEEDED(result));
}

void NSprite::MappingCB()
{
	HRESULT result;

	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);	//マッピング

	assert(SUCCEEDED(result));
}

void NSprite::SetTexHandle(std::string texHandle)
{
	this->texHandle = texHandle;
	if (NTextureManager::GetInstance()->textureMap[texHandle].texBuff == nullptr)
	{
		this->texHandle = "error";
	}
}

void NSprite::SetColor(int R, int G, int B, int A)
{
	XMFLOAT4 color = {};

	color.x = static_cast<float>(R) / 255.0f;
	color.y = static_cast<float>(G) / 255.0f;
	color.z = static_cast<float>(B) / 255.0f;
	color.w = static_cast<float>(A) / 255.0f;
	constMapTransform->color = color;
}

void NSprite::Unmap()
{
	constBuffTransform->Unmap(0, nullptr);
}

void NSprite::UpdateMatrix()
{
	//ワールド行列
	NMatrix4 matRot;	//回転行列
	matRot = matRot.RotateZ(MathUtil::Degree2Radian(rotation));	//Z軸周りに回転

	NMatrix4 matTrans;	//平行移動行列
	matTrans = matTrans.Translation({ position.x, position.y, 0 });

	matWorld = matWorld.Identity();	//単位行列代入
	matWorld *= matRot;		//ワールド座標に回転を反映
	matWorld *= matTrans;	//ワールド座標に平行移動を反映

	TransferMatrix();
}

void NSprite::TransferMatrix()
{
	constMapTransform->mat = matWorld * matProjection;
}

void NSprite::TransferVertex()
{
	//UVだけデフォルトで設定
	VertexUV vert[] = {
		//		u	v
		{{}, {0.0f,1.0f}},	// 左下
		{{}, {0.0f,0.0f}},	// 左上
		{{}, {1.0f,1.0f}},	// 右下
		{{}, {1.0f,0.0f}},	// 右上
	};

	//アンカーポイント設定
	float left = (0.0f - anchorPoint.x) * size.x;
	float right = (1.0f - anchorPoint.x) * size.x;
	float top = (0.0f - anchorPoint.y) * size.y;
	float bottom = (1.0f - anchorPoint.y) * size.y;

	//左右反転
	if (isFlipX)
	{
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY)
	{
		top = -top;
		bottom = -bottom;
	}

	//設定したアンカーポイントに合わせて頂点設定しなおす
	vert[0].pos = { left ,bottom,0.0f };	// 左下
	vert[1].pos = { left ,top   ,0.0f };	// 左上
	vert[2].pos = { right,bottom,0.0f };	// 右下
	vert[3].pos = { right,top   ,0.0f };	// 右上

	//テクスチャサイズをもとに切り取る部分のuvを計算
	float tex_left = texLeftTop.x / resDescVert.Width;
	float tex_right = (texLeftTop.x + texSize.x) / resDescVert.Width;
	float tex_top = texLeftTop.y / resDescVert.Height;
	float tex_bottom = (texLeftTop.y + texSize.y) / resDescVert.Height;

	//計算したuvに合わせて設定しなおす
	vert[0].uv = { tex_left ,tex_bottom };	// 左下
	vert[1].uv = { tex_left ,tex_top };	// 左上
	vert[2].uv = { tex_right,tex_bottom };	// 右下
	vert[3].uv = { tex_right,tex_top };	// 右上

	//設定したら他でも使える変数に代入
	std::copy(std::begin(vert), std::end(vert), vertices);

	//頂点バッファのサイズを代入
	singleSizeVB = static_cast<UINT>(sizeof(vertices[0]));
	sizeVB = static_cast<UINT>(sizeof(vertices));

	//頂点バッファへのデータ転送
	VertMaping();
}

void NSprite::SetSize(float x, float y)
{
	size = { x,y };
	TransferVertex();
}

void NSprite::SetPos(float x, float y)
{
	position = { x,y };
	UpdateMatrix();
}

void NSprite::CommonBeginDraw()
{
	// パイプラインステートとルートシグネチャの設定コマンド
	NDX12::GetInstance()->GetCommandList()->SetPipelineState(PipeLineManager::GetInstance()->GetPipelineSetSprite().pipelineState.Get());
	NDX12::GetInstance()->GetCommandList()->SetGraphicsRootSignature(PipeLineManager::GetInstance()->GetPipelineSetSprite().rootSig.entity.Get());

	// プリミティブ形状の設定コマンド
	NDX12::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形ストリップ

	ID3D12DescriptorHeap* ppHeaps[] = { NDX12::GetInstance()->GetSRVHeap() };
	NDX12::GetInstance()->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void NSprite::Draw()
{
	CommonBeginDraw();

	//非表示フラグ立ってたら描画せずに処理抜ける
	if (isInvisible)
	{
		return;
	}

	//ハンドルを指定
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = NTextureManager::GetInstance()->textureMap[texHandle].gpuHandle;

	//指定のヒープにあるSRVをルートパラメータ1番に設定
	NDX12::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	NDX12::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//ルートパラメータ0番に定数バッファを渡す
	NDX12::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());
	NDX12::GetInstance()->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}