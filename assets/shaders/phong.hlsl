cbuffer cb_per_object : register(b0)
{
  float4x4 World;
  float4x4 WorldNormal;
  float4 ObjectColor;
  Texture2D diffuse;
};

cbuffer cb_per_frame : register(b1)
{
  float4x4 ViewProjection;
  float4 CameraPosition;
  float4 LightPosition;
}

/* vertex attributes go here to input to the vertex shader */
struct vs_in
{
  float3 position_local : POS;
  float3 normal : NOR;
  float2 uv : TEXCOORD;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out
{
  float4 position_clip : SV_POSITION; // required output of VS
  float4 position_world : WPOSITION;
  float4 uv : TEXCOORD;
  float3 normal : NORMAL;
};

vs_out vs_main(vs_in input)
{
  vs_out output;
  output.position_world = mul(World, float4(input.position_local, 1.0f));
  output.position_clip = mul(ViewProjection, output.position_world);
  output.normal = mul(WorldNormal, float4(input.normal, 0.f));
  output.uv = input.uv;
  return output;
}

SamplerState samplerState : SAMPLER : register(s0);

float4 ps_main(vs_out input) : SV_TARGET
{
  const float3 diffCoeff = ObjectColor.rgb + diffuse.Sample(samplerState, input.uv.rg).rgb;
  const float3 light_color = float3(1.0f, 1.0f, 1.0f);
  const float specCoeff = 0.4f;
  const int specExp = 16;
  
  const float3 vHat = normalize(CameraPosition.xyz - input.position_world.xyz);
  const float3 mHat = normalize(input.normal);
  const float3 cAmb = diffCoeff * float3(0.1f, 0.1f, 0.1f);
  const float3 lHat = normalize(LightPosition.xyz - input.position_world.xyz);
  const float mDotL = max(dot(mHat, lHat), 0.f);
  const float3 rlHat = normalize(2.f * mDotL * mHat - lHat);
  const float rlDotV = max(dot(rlHat, vHat), 0.f);
  
  const float3 cDiff = diffCoeff * mDotL * light_color;
  const float3 cSpec = mDotL > 0.f ? specCoeff * pow(rlDotV, specExp) * light_color : float3(0.f, 0.f, 0.f);
  
  return float4(cDiff + cSpec + cAmb, 1.f);
  
  ////L
  //const float3 light_dir = normalize(LightPosition.xyz - input.position_world);
  ////V
  //const float3 view_dir = normalize(CameraPosition.xyz - input.position_world);
  ////m*L
  //const float diffuse_intensity = max(dot(input.normal, light_dir), 0.f);
  ////R
  //  const float3 reflection = diffuse_intensity > 0.f ? normalize(2 * diffuse_intensity * input.normal - light_dir) : float3(0.f, 0.f, 0.f);
  ////(RL*V)^ns
  //const float3 specular_intensity = pow(dot(reflection, view_dir), specIndex);
  ////const float3 reflection = normalize(reflect(normalize(input.normal) ,light_dir));
  ////float specular_intensity = pow(saturate(dot(reflection, view_dir)), 4);
  ////float diffuse_intensity = saturate(dot(normalize(input.normal), light_dir));
  //float3 ambient_color = float3(0.01f, 0.01f, 0.01f) * ObjectColor.rgb;
  //  float3 color = ambient_color + (diffuse_intensity * ObjectColor.rgb) + (specCoeff * specular_intensity * light_color);
  //return float4(color, 1.0); // must return an RGBA color
}
