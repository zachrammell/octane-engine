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
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
  float4 position_ndc : SV_POSITION; // required output of VS
  float4 position_tex : TEXCOORD0; //Todo: implement this
};

vs_out vs_main(vs_in input)
{
  vs_out output;
  
  output.position_ndc = mul(NDC, float4(input.position_ndc.xy,0.f, input.position_ndc.z));
  return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
  return ObjectColor;
}
