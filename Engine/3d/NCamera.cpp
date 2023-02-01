#include "NCamera.h"
#include "NWindows.h"
#include "NMathUtil.h"

std::unique_ptr<NCamera> NCamera::_nCamera = std::make_unique<NCamera>();
NCamera* NCamera::nowCamera = _nCamera.get();

void NCamera::CreateMatView()
{
	matView = MathUtil::MatView(eye, target, up);
	/*XMMATRIX mat = XMMatrixLookAtLH(XMVECTOR({eye.x,eye.y,eye.z,0}), XMVECTOR({ target.x,target.y,target.z,0 }) , XMVECTOR({ up.x,up.y,up.z,0 }) );
	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			matView.m[i][j] = mat.r[i].m128_f32[j];
		}
	}*/
}

void NCamera::ProjectiveProjection()
{
	matProjection = MathUtil::PerspectiveProjection(
		MathUtil::Degree2Radian(45.0f),
		(float)NWindows::win_width / NWindows::win_height,
		0.1f, 1000.0f);
}
