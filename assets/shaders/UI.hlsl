cbuffer cb_per_object : register(b0)
{
  float4x4 NDC;
  float4x4 padding;
  float4 ObjectColor;
};

/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
  float3 position_ndc : POS; //x and y between 0 and 1
  float2 uv : TEX;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
  float4 position_ndc : SV_POSITION; // required output of VS
  float2 uv : TEXCOORD; //Todo: implement this
};

vs_out vs_main(vs_in input)
{
  vs_out output;
  
  output.position_ndc = mul(NDC, float4(input.position_ndc.xyz, 1.f));
  output.uv = input.uv;
  return output;
}

Texture2D diffuse : DIFFUSE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 ps_main(vs_out input) : SV_TARGET
{
  return ObjectColor + diffuse.Sample(samplerState, input.uv).rgba;
}
