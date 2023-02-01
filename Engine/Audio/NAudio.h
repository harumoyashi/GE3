#pragma once
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")

#include <array>
#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <fstream>
#include <wrl.h>

//音声データ
struct SoundData
{
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;
	//名前
	std::string name_;
};

// 再生データ
struct Voice {
	uint32_t handle = 0u;
	IXAudio2SourceVoice* sourceVoice = nullptr;
};

class NAudio
{
private:
	// サウンドデータの最大数
	static const int kMaxSoundData = 256;

	//チャンクヘッダー
	struct ChunkHeader
	{
		char id[4];		//チャンク毎のID
		int32_t size;	//チャンクサイズ
	};

	//RIFFヘッダーチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMATEX fmt;	//波形フォーマット
	};

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	std::ifstream file;			//ファイル入力ストリームのインスタンス
	FormatChunk format = {};	//フォーマットチャンク
	ChunkHeader data;			//データチャンク
	char* pBuffer;
	SoundData soundData = {};	//音声データ

	//オーディオコールバック
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		// ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd)(THIS) {};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext);
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBufferContext) {};
		// ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};
	};

public:
	static NAudio* GetInstance();

	//初期化
	//ディレクトリパスを指定しないと"Resources/sound/"になる
	void Init(const std::string& directoryPath = "Resources/sound/");

	//終了処理
	void Finalize();
	//WAV音声読み込み
	//"filename" = WAVファイル名
	//return サウンドデータハンドル
	uint32_t LoadWave(const std::string& filename);

	//サウンドデータの解放
	void Unload(SoundData* soundData);


	//音声再生
	//"soundDataHandle" サウンドデータハンドル
	//"loopFlag" ループ再生フラグ
	//"volume" ボリューム
	//0で無音、1がデフォルト音量。あまり大きくしすぎると音割れする
	//return 再生ハンドル
	uint32_t PlayWave(uint32_t soundDataHandle, bool loopFlag = false, float volume = 1.0f);

	//音声削除
	//"voiceHandle" 再生ハンドル
	void DestroyWave(uint32_t voiceHandle);

	//音声一時停止
	//"voiceHandle" 再生ハンドル
	void StopWave(uint32_t voiceHandle);

	//一時停止した音声を再生
	//"voiceHandle" 再生ハンドル
	void StartWave(uint32_t voiceHandle);

	//音声再生中かどうか
	//"voiceHandle" 再生ハンドル
	bool IsPlaying(uint32_t voiceHandle);

	//音量設定
	//"voiceHandle" 再生ハンドル
	//"volume" ボリューム
	//0で無音、1がデフォルト音量。あまり大きくしすぎると音割れする
	void SetVolume(uint32_t voiceHandle, float volume);

private:
	// XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	// サウンドデータコンテナ
	std::array<SoundData, kMaxSoundData> soundDatas_;
	// 再生中データコンテナ
	// std::unordered_map<uint32_t, IXAudio2SourceVoice*> voices_;
	std::set<Voice*> voices_;
	// サウンド格納ディレクトリ
	std::string directoryPath_;
	// 次に使うサウンドデータの番号
	uint32_t indexSoundData_ = 0u;
	// 次に使う再生中データの番号
	uint32_t indexVoice_ = 0u;
	// オーディオコールバック
	XAudio2VoiceCallback voiceCallback_;
};