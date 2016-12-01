texture2D baseTexture : register(t0); // first texture

SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP


// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 uv : UV;
	float3 norm : NORMAL;
	float3 worldPos : WORLD_POS;
};

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer DIR_LIGHT : register(b0)
{
	float4 dir_color;
	float4 dir;
};

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer POINT_LIGHT : register(b1)
{
	float4 point_color;
	float4 point_pos;
	float4 point_radius;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters[0], input.uv);
	float3 AmbientLight = float3(0.25f, 0.25f, 0.25f) * baseColor.xyz;

	float3 lightDir = -normalize(dir);
	float3 wnrm = normalize(input.norm);

	float3 directionalLight = saturate((dot(lightDir, wnrm)) * baseColor.xyz * dir_color.xyz);
	float4 finalColor = float4(saturate(AmbientLight + directionalLight), baseColor.w);

	float3 pointLightDir = normalize(point_pos.xyz - input.worldPos);
	float pointLightRatio = saturate(dot(pointLightDir, wnrm));
	float4 result = pointLightRatio * point_color * baseColor;

	return saturate(finalColor + result);
}
