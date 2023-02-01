#include "NAudio.h"

#include <cassert>
#include <fstream>
#include <windows.h>

#pragma comment(lib, "xAudio2.lib")

void NAudio::XAudio2VoiceCallback::OnBufferEnd(THIS_ void* pBufferContext) {

	Voice* voice = reinterpret_cast<Voice*>(pBufferContext);
	// 再生リストから除外
	NAudio::GetInstance()->voices_.erase(voice);
}

NAudio* NAudio::GetInstance() {
	static NAudio instance;

	return &instance;
}

void NAudio::Init(const std::string& directoryPath) {
	directoryPath_ = directoryPath;

	HRESULT result;
	IXAudio2MasteringVoice* masterVoice;

	// XNAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	indexSoundData_ = 0u;
	indexVoice_ = 0u;
}

void NAudio::Finalize() {
	// XAudio2解放
	xAudio2_.Reset();
	// 音声データ解放
	for (auto& soundData : soundDatas_) {
		Unload(&soundData);
	}
}

uint32_t NAudio::LoadWave(const std::string& fileName) {
	assert(indexSoundData_ < kMaxSoundData);
	uint32_t handle = indexSoundData_;
	// 読み込み済みサウンドデータを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData.name_ == fileName;
		});
	if (it != soundDatas_.end()) {
		// 読み込み済みサウンドデータの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
		return handle;
	}

	// ディレクトリパスとファイル名を連結してフルパスを得る
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullpath = currentRelative ? fileName : directoryPath_ + fileName;

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(fullpath, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクか Broadcast Wave Formatを検出した場合。
	while (_strnicmp(data.id, "junk", 4) == 0 || _strnicmp(data.id, "bext", 4) == 0 ||
		_strnicmp(data.id, "LIST", 4) == 0 || _strnicmp(data.id, "FLLR", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (_strnicmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	// 書き込むサウンドデータの参照
	SoundData& soundData = soundDatas_.at(handle);

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.name_ = fileName;

	//実質的にハンドルを次に進める
	indexSoundData_++;

	return handle;
}

void NAudio::Unload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

uint32_t NAudio::PlayWave(uint32_t soundDataHandle, bool loopFlag, float volume) {
	HRESULT result;
	IXAudio2SourceVoice* pSourceVoice = nullptr;

	assert(soundDataHandle <= soundDatas_.size());

	// サウンドデータの参照を取得
	SoundData& soundData = soundDatas_.at(soundDataHandle);
	// 未読み込みの検出
	assert(soundData.bufferSize != 0);

	uint32_t handle = soundDataHandle;

	// 波形フォーマットを元にSourceVoiceの生成
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex, 0, 2.0f, &voiceCallback_);
	assert(SUCCEEDED(result));

	// 再生中データ
	Voice* voice = new Voice();
	voice->handle = soundDataHandle;
	voice->sourceVoice = pSourceVoice;
	// 再生中データコンテナに登録
	voices_.insert(voice);

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.pContext = voice;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loopFlag) {
		// 無限ループ
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();

	return handle;
}

void NAudio::DestroyWave(uint32_t voiceHandle)
{
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) {
			return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->DestroyVoice();

		voices_.erase(it);
	}
}

void NAudio::StopWave(uint32_t voiceHandle) {

	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->Stop();
	}
}

void NAudio::StartWave(uint32_t voiceHandle)
{
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		// 再生再開
		(*it)->sourceVoice->Start();
	}
}

bool NAudio::IsPlaying(uint32_t voiceHandle) {
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見。再生終わってるのかどうかを判断
	if (it != voices_.end()) {
		XAUDIO2_VOICE_STATE state{};
		(*it)->sourceVoice->GetState(&state);
		return state.SamplesPlayed != 0;
	}
	return false;
}

void NAudio::SetVolume(uint32_t voiceHandle, float volume) {
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->SetVolume(volume);
	}
}
