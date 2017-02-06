cbuffer MatricesBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix viewInverse;
	matrix projMatrix;
};

cbuffer NoiseBuffer
{
    float animTime;
    float3 scrollSpeeds;
    float3 scales;
    float padding;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 colour : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
	float4 colour : COLOR;
};

PixelInputType vs_main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	
	float3 vx = viewInverse[0].xyz;

	input.position.xz = input.position.x * vx.xz;	//billboard limited to xz to prevent tilting

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);

	output.texCoord = input.texCoord;

	//texcoords for multi-sampling noise texture using the scale and speed values.
    output.texCoords1 = (input.texCoord * scales.x);
    output.texCoords1.y = output.texCoords1.y + (animTime * scrollSpeeds.x);

    output.texCoords2 = (input.texCoord * scales.y);
    output.texCoords2.y = output.texCoords2.y + (animTime * scrollSpeeds.y);

    output.texCoords3 = (input.texCoord * scales.z);
    output.texCoords3.y = output.texCoords3.y + (animTime * scrollSpeeds.z);

	output.colour = input.colour;

	return output;
}