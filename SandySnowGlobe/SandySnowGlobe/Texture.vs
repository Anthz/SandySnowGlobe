cbuffer MatricesBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
};

PixelInputType vs_main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.texCoord = input.texCoord;

	return output;
}