cbuffer MatricesBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 colour : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

PixelInputType vs_main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.colour = input.colour;

	return output;
}