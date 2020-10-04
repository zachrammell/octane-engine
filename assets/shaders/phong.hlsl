cbuffer cb_per_object : register(b0)
{
  float4x4 World;
  float4x4 WorldNormal;
  float4   ObjectColor;
};

cbuffer cb_per_frame : register(b1)
{
  float4x4 ViewProjection;
  float4   CameraPosition;
  float4   LightPosition;
}

/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
  float3 position_local : POS;
  float3 normal : NOR;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
  float4 position_clip : SV_POSITION; // required output of VS
  float4 position_world : TEXCOORD0;
  float3 normal : COLOR0;
};

vs_out vs_main(vs_in input)
{
  vs_out output;
  output.position_world = mul(float4(input.position_local, 1.0f), World);
  output.position_clip = mul(output.position_world, ViewProjection);
  output.normal = mul(input.normal, WorldNormal);
  return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
  const float3 light_dir = normalize(-LightPosition.xyz - input.position_world);
  const float3 view_dir = normalize(CameraPosition.xyz - input.position_world);
  const float3 reflection = normalize(reflect(light_dir, normalize(input.normal)));
  float specular_intensity = pow(saturate(dot(reflection, view_dir)), 4);
  float diffuse_intensity = saturate(dot(normalize(input.normal), light_dir));
  float3 ambient_color = float3(0.01f, 0.01f, 0.01f);
  float3 light_color = float3(1, 1, 1);
  float3 color = ambient_color + (diffuse_intensity * ObjectColor.rgb) + (specular_intensity * light_color);
  return float4(color, 1.0); // must return an RGBA color
}
