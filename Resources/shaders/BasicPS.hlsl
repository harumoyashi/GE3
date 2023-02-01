#include "Basic.hlsli"

float4 main() : SV_TARGET
{
	return color;
}

Texture2D<float4>tex:register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);	//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float3 light = normalize(float3(1,-1,1));				//右下奥向きのライト
	float3 diffuse = saturate(dot(-light, input.normal));	//diffuseを[0,1]の範囲にClampする
	float brightness = diffuse + 0.3f;						//アンビエント項を0.3として計算

	float4 texcolor = float4(tex.Sample(smp, input.uv));
    //return texcolor;

	return float4(texcolor.rgb * brightness, texcolor.a) * color;	//輝度をRGBに代入して出力
	//return float4(1,1,1,1);	//白く塗りつぶす
}