cbuffer WorldMatrixBuffer : register(b0)
{
	matrix worldMatrix;
};

cbuffer ViewMatrixBuffer : register(b1)
{
	matrix viewMatrix;
};

cbuffer ProjectionMatrixBuffer : register(b2)
{
	matrix projectionMatrix;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
	OutputType output;
	
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
    output.tex = input.tex;
    output.normal = input.normal;
	
	return output;
}