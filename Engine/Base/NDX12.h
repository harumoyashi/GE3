#pragma once
#include "NWindows.h"
#include <DirectXTex.h>

#include <d3d12.h>
#include <dxgi1_6.h>	//Visual Studio Graphics Debugger���듮��N��������A1_4�ɂ���Ɖ������邩��

#include <wrl.h>

#include <DirectXMath.h>
#include <chrono>
using namespace DirectX;

#include "NInput.h"

class NDX12
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//�f�o�b�O���C���[���I���ɂ��邽�߂Ɏg�p�����C���^�[�t�F�C�X
	ComPtr<ID3D12Debug> debugController;

	//��{�I�u�W�F�N�g�̐���
	//�󂯎M�ɂȂ�ϐ�����
	ComPtr<ID3D12Device> device;						//�F�X�Ȃ��̍쐬���邽�߂̉��z�A�_�v�^	
	ComPtr<IDXGIFactory6> dxgiFactory;					//�A�_�v�^�[�̗񋓏�Ԃ̕ω������o�ł���悤�ɂ��邽�߂̃C���^�[�t�F�[�X	
	ComPtr<IDXGISwapChain4> swapchain;					//�_�u���o�b�t�@�����O�̂��߂ɉ�ʐ؂�ւ��p�̃o�b�t�@�[�Ǘ�������	
	ComPtr<ID3D12CommandAllocator> commandAllocator;	//�R�}���h���X�g�Ɋi�[���閽�߂ׂ̈̃��������Ǘ�����I�u�W�F�N�g	
	ComPtr<ID3D12GraphicsCommandList> commandList;		//���߂��ꎞ�I�Ɋi�[���Ƃ����
	ComPtr<ID3D12CommandQueue> commandQueue;			//GPU�����s����ׂ����߂̃��X�g��,�w�肵��������GPU�ɓ]�����邽�߂̃C���^�[�t�F�[�X

	ComPtr<ID3D12DescriptorHeap> rtvHeap;

	std::vector< ComPtr<IDXGIAdapter4>> adapters;	//�A�_�v�^�[�̗񋓗p
	ComPtr<IDXGIAdapter4> tmpAdapter;				//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������

	D3D_FEATURE_LEVEL featureLevel;

	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};

	ComPtr<ID3D12DescriptorHeap> srvHeap;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;		//�����_�[�^�[�Q�b�g�r���[�n���h��
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};	//�����_�[�^�[�Q�b�g�r���[�̐ݒ�

	D3D12_RESOURCE_DESC depthResourceDesc{};	//�[�x�o�b�t�@���\�[�X
	D3D12_HEAP_PROPERTIES depthHeapProp{};		//�q�[�v�v���p�e�B
	D3D12_CLEAR_VALUE depthClearValue{};
	ComPtr<ID3D12Resource> depthBuff;			//�[�x�o�b�t�@
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};	//�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	ComPtr<ID3D12Fence> fence;	//CPU��GPU�̓����Ɏg������
	UINT64 fenceVal = 0;

	//fps�Œ�p
	std::chrono::steady_clock::time_point reference;	//�L�^����
	const long long fps = 60;	//60FPS

public:
	std::vector<ComPtr<ID3D12Resource>> backBuffers;

public:
	//�V���O���g���C���X�^���X�擾
	static NDX12* GetInstance();

	//DirectX������
	void Init(NWindows* win);
	//�`��㏈��
	void PostDraw(D3D12_RESOURCE_BARRIER barrierDesc);

	//FPS�Œ�X�V
	//divideFrameRate:�t���[�����[�g��������1�ɂ��邩
	void UpdateFixFPX(float divideFrameRate = 1.0f);

	//�Q�b�^�[//
	//�f�o�C�X�擾
	ID3D12Device* GetDevice()const { return device.Get(); }
	//�X���b�v�`�F�[���擾
	IDXGISwapChain4* GetSwapchain()const { return swapchain.Get(); }
	//�R�}���h�A���P�[�^�[�擾
	ID3D12CommandAllocator* GetCommandAllocator()const { return commandAllocator.Get(); }
	//�R�}���h���X�g�擾
	ID3D12GraphicsCommandList* GetCommandList()const { return commandList.Get(); }
	//�R�}���h�L���[�擾
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue.Get(); }
	//RTV�q�[�v�擾
	ID3D12DescriptorHeap* GetRTVHeap()const { return rtvHeap.Get(); }
	//SRV�q�[�v�擾
	ID3D12DescriptorHeap* GetSRVHeap()const { return srvHeap.Get(); }
	//RTV�q�[�v�f�X�N�擾
	D3D12_DESCRIPTOR_HEAP_DESC GetRTVHeapDesc()const { return rtvHeapDesc; }
	//DSV�q�[�v�擾
	ID3D12DescriptorHeap* GetDSVHeap()const { return dsvHeap.Get(); }
	//�t�F���X�擾
	ID3D12Fence* GetFence()const { return fence.Get(); }

private:
	//�A�_�v�^�[�I��
	void ChoiceAdapters();
	//�f�o�C�X�̐���
	void CreateDevice();
	//�R�}���h�A���P�[�^�A���X�g�A�L���[�̐���
	void CreateCommandGroup();
	// �X���b�v�`�F�[���̐���
	void CreateSwapChain(NWindows* win);
	//�V�F�[�_���\�[�X�r���[�̃f�X�N���v�^�q�[�v����
	void CreateSRVHeapDesc();
	//�����_�[�^�[�Q�b�g�r���[�̃f�X�N���v�^�q�[�v����
	void CreateRTVHeapDesc();
	//�����_�[�^�[�Q�b�g�r���[�̐���
	void CreateRTV();
	//�[�x�o�b�t�@�̃��\�[�X�ݒ�
	void SetDepthRes();
	//�[�x�o�b�t�@����
	void CreateDepthBuff();
	//�[�x�r���[�p�f�X�N���v�^�q�[�v����
	void CreateDescHeap();
	//�[�x�r���[�쐬
	void CreateDSV();
	//�t�F���X�̐���
	void CreateFence();

	//FPS�Œ菉����
	void InitializeFixFPS();

	//�o���A����
	void BarrierReset(D3D12_RESOURCE_BARRIER barrierDesc);
	//���߂̃N���[�Y
	//�����R�}���h���X�g�ɐςނ̂����܂�
	void CmdListClose();
	// �R�}���h���X�g�̎��s
	void ExecuteCmdList();
	// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
	void BufferSwap();
	// �R�}���h�̎��s������҂�
	void CommandWait();
	// �L���[���N���A
	void ClearQueue();
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	void CmdListReset();
};
