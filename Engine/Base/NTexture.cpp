#include "NTexture.h"

NTextureManager* NTextureManager::GetInstance()
{
	static NTextureManager instance;
	return &instance;
}

void NTextureManager::Init()
{
	CreateErrorTexture();
	//ここで一括読み込み
	LoadTexture("Resources/white64x64.png", "white");
	LoadTexture("Resources/itiro_kimegao.png", "itiro");
	LoadTexture("Resources/hamutaro.jpg", "hamu");
	LoadTexture("Resources/mario.jpg", "mario");
}

NTexture NTextureManager::CreateErrorTexture()
{
	if (textureMap["error"].texBuff == nullptr)
	{
		NTexture texture;	//成果物一時保存用

		//一辺のピクセル数
		const size_t textureLen = 256;
		//配列の要素数
		const size_t imageDataCount = textureLen * textureLen;
		//画像イメージデータ配列
		XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];	//※必ず後で解放する

		//全ピクセルの色を初期化
		for (size_t i = 0; i < imageDataCount; i++)
		{
			size_t x = i % textureLen;
			size_t y = i / textureLen;

			//左上と右下をピンクに
			if ((x < textureLen / 2 && y < textureLen / 2)
				|| (x >= textureLen / 2 && y >= textureLen / 2)) {
				imageData[i] = { 1, 0, 1, 1 };
			}
			else {	//それ以外は黒に
				imageData[i] = { 0, 0, 0, 1 };
			}
		}

		//テクスチャバッファ設定
		SetTBHeap();
		texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texResDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texResDesc.Width = textureLen;	//幅
		texResDesc.Height = textureLen;	//高さ
		texResDesc.DepthOrArraySize = 1;
		texResDesc.MipLevels = 1;
		texResDesc.SampleDesc.Count = 1;

		HRESULT result;
		result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&texResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texture.texBuff)
		);

		//テクスチャバッファにデータ転送
		result = texture.texBuff->WriteToSubresource(
			0,
			nullptr,	//全領域へコピー
			imageData,	//元データアドレス
			sizeof(XMFLOAT4) * textureLen,	//1ラインサイズ
			sizeof(XMFLOAT4) * imageDataCount	//全サイズ
		);

		//元データ解放
		delete[] imageData;


		//シェーダーリソースビュー設定
		SetSRV();
		texture = CreateSRV(texture);

		//名前登録
		texture.fileName = "error";

		//最後に成果物を格納
		textureMap["error"] = texture;

		return texture;
	}

	return textureMap["error"];
}

NTexture NTextureManager::LoadTexture(const std::string pictureName, const std::string handle)
{
	if (textureMap[handle].texBuff == nullptr)
	{
		NTexture texture;	//成果物一時保存用

		if (!Load(pictureName))
		{
			return CreateErrorTexture();
		}

		CreateMipmap();
		//テクスチャバッファ設定
		SetTBHeap();
		SetTBResource();
		texture.texBuff = CreateTexBuff();
		TexBuffMaping(texture.texBuff.Get());
		//シェーダーリソースビュー設定
		SetSRV();
		texture = CreateSRV(texture);

		//名前登録
		texture.fileName = handle;

		//最後に成果物を格納
		textureMap[handle] = texture;

		return texture;
	}
	return textureMap[handle];
}

bool NTextureManager::Load(const std::string pictureName)
{
	HRESULT result;
	//一回別の文字列型に変換して
	std::wstring wPictureName(pictureName.begin(), pictureName.end());

	result = LoadFromWICFile(
		wPictureName.c_str(),	//ここで文字型に
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	if (result != S_OK)
	{
		return false;
	}
	return true;
}

void NTextureManager::CreateMipmap()
{
	HRESULT result;

	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);
}

void NTextureManager::SetTBHeap()
{
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//転送はCPU側(L0)から直接行う
	texHeapProp.CreationNodeMask = 0;							//単一アダプターだから0
	texHeapProp.VisibleNodeMask = 0;
}

void NTextureManager::SetTBResource()
{
	texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResDesc.Format = metadata.format;

	texResDesc.Width = metadata.width;	//幅
	texResDesc.Height = (UINT)metadata.height;	//高さ
	texResDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texResDesc.MipLevels = (UINT16)metadata.mipLevels;
	texResDesc.SampleDesc.Count = 1;
}

ID3D12Resource* NTextureManager::CreateTexBuff()
{
	HRESULT result;

	result = NDX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&tb)
	);

	return tb.Get();
}

void NTextureManager::TexBuffMaping(ID3D12Resource* texBuff)
{
	HRESULT result;

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//全サイズ
		);
		assert(SUCCEEDED(result));
	}
}

void NTextureManager::SetSRV()
{
	srvDesc.Format = texResDesc.Format;		//RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = texResDesc.MipLevels;
}

NTexture NTextureManager::CreateSRV(NTexture tex)
{
	//SRVヒープの先頭ハンドルを取得
	tex.cpuHandle = NDX12::GetInstance()->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	tex.gpuHandle = NDX12::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	//指定されたテクスチャ番号に応じてハンドルを進める
	UINT incrementSize = NDX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	tex.cpuHandle.ptr += incrementSize * (UINT)textureMap.size();
	tex.gpuHandle.ptr += incrementSize * (UINT)textureMap.size();

	//ハンドルの指す位置にシェーダーリソースビュー作成
	NDX12::GetInstance()->GetDevice()->CreateShaderResourceView(tex.texBuff.Get(), &srvDesc, tex.cpuHandle);

	return tex;
}

NTexture::NTexture()
{
}

NTexture::~NTexture()
{
}
