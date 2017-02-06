cbuffer MatricesBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix viewInverse;
	matrix projMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 colour : COLOR;
	float3 instancePos : TEXCOORD1;
	float3 instanceRot : TEXCOORD2;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 colour : COLOR;
};

PixelInputType vs_main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	//output.position = mul(input.position, worldMatrix);

	output.position = mul(input.position, viewInverse);

	output.position.x += input.instancePos.x;
	output.position.y += input.instancePos.y;
	output.position.z += input.instancePos.z;

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);

	output.texCoord = input.texCoord;
	output.colour = input.colour;

	return output;
}