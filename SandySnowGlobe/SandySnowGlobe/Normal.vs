cbuffer MatricesBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInputType vs_main(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.texCoord = input.texCoord;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);
	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);

	return output;
}