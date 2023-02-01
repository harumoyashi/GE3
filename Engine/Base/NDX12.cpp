#include "NDX12.h"
#include <thread>

NDX12* NDX12::GetInstance()
{
	static NDX12 instance;
	return &instance;
}

void NDX12::Init(NWindows* win)
{
	HRESULT result;

#ifdef _DEBUG
	//デバッグレイヤーをオンに
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif
	InitializeFixFPS();

	// DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	ChoiceAdapters();
	CreateDevice();
	CreateCommandGroup();
	CreateSwapChain(win);
	CreateSRVHeapDesc();
	CreateRTVHeapDesc();
	CreateRTV();
	SetDepthRes();
	CreateDepthBuff();
	CreateDescHeap();
	CreateDSV();
	CreateFence();
}

void NDX12::PostDraw(D3D12_RESOURCE_BARRIER barrierDesc)
{
	BarrierReset(barrierDesc);
	CmdListClose();
	ExecuteCmdList();
	BufferSwap();
	CommandWait();
	UpdateFixFPX();
	ClearQueue();
	CmdListReset();
}

void NDX12::ChoiceAdapters()
{
	// パフォーマンスが高いものから順に、全てのアダプターを列挙する
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		// 動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	// 妥当なアダプタを選別する
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		// アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);
		// ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}
}

void NDX12::CreateDevice()
{
	HRESULT result;

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	};

	//生成可能なバージョンが見つかったら打ち切り
	for (size_t i = 0; i < _countof(levels); i++) {
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK) {	//HRESULT型の関数はtrue,falseの代わりにS_OK,特定の型が返される
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);	//やばいエラーの時止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);		//エラーの時止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);	//警告の時止まる
	}

	//抑制するエラー
	D3D12_MESSAGE_ID denyIds[] = {
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};

	//抑制される表示レベル
	D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(severities);
	filter.DenyList.pSeverityList = severities;
	//指定したエラーの表示を抑制する
	infoQueue->PushStorageFilter(&filter);
#endif // DEBUG
}

void NDX12::CreateCommandGroup()
{
	HRESULT result;

	// コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));

	// コマンドリストを生成
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューを生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
}

void NDX12::CreateSwapChain(NWindows* win)
{
	HRESULT result;

	swapChainDesc.Width = NWindows::win_width;
	swapChainDesc.Height = NWindows::win_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapChainDesc.BufferCount = 2; // バッファ数を2つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//一時的なComPtr
	ComPtr<IDXGISwapChain1> swapchain1;

	// スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(), win->GetHwnd(), &swapChainDesc, nullptr, nullptr, &swapchain1);
	assert(SUCCEEDED(result));

	//もとのスワップチェーンに変換
	swapchain1.As(&swapchain);
}

void NDX12::CreateSRVHeapDesc()
{
	HRESULT result;

	//シェーダリソースビュー(SRV)の最大個数
	const size_t kMaxSRVCount = 2056;
	// デスクリプタヒープの設定
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	//シェーダから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;	//デスクリプタの持てる数設定

	//設定をもとにSRV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}

void NDX12::CreateRTVHeapDesc()
{
	// デスクリプタヒープの設定
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; // 裏表の2つ
	// デスクリプタヒープの生成
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
}

void NDX12::CreateRTV()
{
	// バックバッファ
	backBuffers.resize(swapChainDesc.BufferCount);

	// スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++) {
		// スワップチェーンからバッファを取得
		swapchain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		// デスクリプタヒープのハンドルを取得
		rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		// 裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}
}

void NDX12::SetDepthRes()
{
	//リソース設定
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = NWindows::win_width;	//レンダーターゲットに合わせる
	depthResourceDesc.Height = NWindows::win_height;	//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//デプスステンシル

	//深度値用ヒーププロパティ
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	depthClearValue.DepthStencil.Depth = 1.0f;	//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
}

void NDX12::CreateDepthBuff()
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);
}

void NDX12::CreateDescHeap()
{
	HRESULT result;

	dsvHeapDesc.NumDescriptors = 1;	//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//デプスステンシルビュー

	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
}

void NDX12::CreateDSV()
{
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void NDX12::CreateFence()
{
	HRESULT result;

	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(result));
}

void NDX12::InitializeFixFPS()
{
	//現在の時間を記録する
	reference = std::chrono::steady_clock::now();
}

void NDX12::UpdateFixFPX(float divideFrameRate)
{
	//1/60秒(1フレーム)ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / (60.0f / divideFrameRate)));
	//1/60秒よりちょっとだけ短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / (65.0f / divideFrameRate)));

	//現在の時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回の記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference);

	//1/60秒(よりちょっとだけ短い時間)経ってない場合
	if (elapsed<kMinCheckTime)
	{
		//1/60秒経過するまで細かいスリープを繰り返す
		while (std::chrono::steady_clock::now()-reference<kMinTime)
		{
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録する
	reference = std::chrono::steady_clock::now();
}

void NDX12::BarrierReset(D3D12_RESOURCE_BARRIER barrierDesc)
{
	// 5.リソースバリアを戻す
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//描画状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//表示状態へ
	commandList->ResourceBarrier(1, &barrierDesc);
}

void NDX12::CmdListClose()
{
	HRESULT result;

	result = commandList->Close();
	assert(SUCCEEDED(result));
}

void NDX12::ExecuteCmdList()
{
	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);
}

void NDX12::BufferSwap()
{
	HRESULT result;

	result = swapchain->Present(1, 0);
	assert(SUCCEEDED(result));
}

void NDX12::CommandWait()
{
	commandQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

void NDX12::ClearQueue()
{
	HRESULT result;

	result = commandAllocator->Reset();
	assert(SUCCEEDED(result));
}

void NDX12::CmdListReset()
{
	HRESULT result;

	result = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));
}