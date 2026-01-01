Texture2D tex0 : register(t0);
SamplerState samp0 : register(s0);

cbuffer ColourBuffer : register(b0)
{
    float4 colourTint;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    return tex0.Sample(samp0, input.tex) * colourTint;
}