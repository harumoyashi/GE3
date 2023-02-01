#include "NWindows.h"

#pragma comment(lib,"winmm.lib")

NWindows* NWindows::GetInstance()
{
	static NWindows instance;
	return &instance;
}

//ウィンドプロシージャ
//面倒だけど書かなきゃいけない関数
LRESULT NWindows::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		//ウィンドウが破棄されたら呼ばれる
	case WM_DESTROY:
		PostQuitMessage(0);	//OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	//既定の処理を行う
}

//Windowクラスの設定
void NWindows::Set()
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;		//ウィンドウプロシージャを設定
	w.lpszClassName = L"DX12Sample";			//アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定

	RegisterClassEx(&w);	//アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）

	wrc = { 0,0,win_width,win_height };	//ウィンドウサイズを決める

	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//システムタイマーの分解度を上げる
	timeBeginPeriod(1);
}

//コンソールへの文字出力
void NWindows::DebugText(LPCSTR text)
{
	OutputDebugStringA(text);
}

//ウィンドウオブジェクトの生成
void NWindows::CreateWindowObj()
{
	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		w.lpszClassName,		//クラス名指定
		L"DX12テスト",			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示x座標はOSにお任せ
		CW_USEDEFAULT,			//表示y座標はOSにお任せ
		wrc.right - wrc.left,	//ウィンドウ幅
		wrc.bottom - wrc.top,	//ウィンドウ高
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//追加パラメーター
	);
}

//ウィンドウ表示
void NWindows::Display()
{
	ShowWindow(hwnd, SW_SHOW);
}

bool NWindows::WindowMessage()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);	//キー入力メッセージの処理
		DispatchMessage(&msg);	//プロシージャにメッセージを送る
	}

	//アプリケーションが終わるときにmwssageがWM_QUITになる
	if (msg.message == WM_QUIT)
	{
		return true;	//終了メッセージが来たらループを抜ける
	}

	return false;
}

void NWindows::Finalize()
{
	//もうクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);
}