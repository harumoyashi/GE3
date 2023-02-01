#pragma once
#include "NDX12.h"

#include <wrl.h>

class NPreDraw
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	UINT bbIndex;
	D3D12_RESOURCE_BARRIER barrierDesc{};		//リソースバリア
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};	//レンダーターゲットビューハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};	//デスクリプタハンドル

	D3D12_VIEWPORT viewport{};	//ビューポート
	D3D12_RECT scissorRect{};	//シザー矩形

public:
	//リソースバリアで書き込み可能に変更
	void SetResBarrier();
	//描画先の変更
	void SetRenderTarget();
	//画面クリアRGBA
	void ClearScreen();
	//ビューポート設定
	void SetViewport();
	//シザー矩形設定
	void SetScissorRect();

	//バリア解除
	void BarrierReset(D3D12_RESOURCE_BARRIER barrierDesc);
	//命令のクローズ
	//もうコマンドリストに積むのおしまい
	void CmdListClose();
	// コマンドリストの実行
	void ExecuteCmdList();
	// 画面に表示するバッファをフリップ(裏表の入替え)
	void BufferSwap();
	// コマンドの実行完了を待つ
	void CommandWait(UINT64 fenceVal);
	// キューをクリア
	void ClearQueue();
	// 再びコマンドリストを貯める準備
	void CmdListReset();
};

