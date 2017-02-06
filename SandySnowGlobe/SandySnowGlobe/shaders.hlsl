static const float4x4 xform_identity = float4x4(1, 0, 0, 0,
												0, 1, 0, 0,
												0, 0, 1, 0,
												0, 0, 0, 1);

struct vs_input {
	float3 pos : POSITION;
	float3 nrm : NORMAL;
	float4 col : COLOR;
};

struct vs_output {
	float4 pos : SV_POSITION;
	float4 nrm : NORMAL;
	float4 col : COLOR;
};

typedef vs_output ps_input;

struct ps_output {
	float4 col : SV_TARGET;
};

cbuffer cb_per_shader_invocation {
	float4x4 xform_projection;
	float4x4 xform_projection_it;
};

cbuffer cb_per_frame {
	float4x4 xform_view;
	float4x4 xform_view_it;
};

cbuffer cb_per_primitive {
	float4x4 xform_model;
	float4x4 xform_model_it;
};

vs_output
vs_main(vs_input input)
{
	vs_output result;

	float4x4 mvp    = xform_identity; // xform_projection    * xform_view    * xform_model;
	float4x4 mvp_it = xform_identity; // xform_projection_it * xform_view_it * xform_model_it;

	result.pos = mul(mvp,    float4(input.pos, 1.0));
	result.nrm = mul(mvp_it, float4(input.nrm, 0.0));
	result.col = input.col;

	return result;
}

ps_output
ps_main(ps_input input)
{
	ps_output result = { float4(input.col.rgb, 1.0) };

	return result;
}
