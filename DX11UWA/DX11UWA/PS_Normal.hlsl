texture2D baseTexture : register(t0); // first texture

texture2D detailTexture : register(t1);

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

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer SPOT_LIGHT : register(b2)
{
    float4 spot_color;
    float4 spot_pos;
    float4 spot_coneDir;
    float4 spot_coneRatio;
    float4 spot_InnerRatio;
    float4 spot_OuterRatio;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 baseColor = baseTexture.Sample(filters[0], input.uv);
    float4 detailColor = detailTexture.Sample(filters[1], input.uv);
    detailColor.x = (detailColor.x * 2.0f) - 1.0f;
    detailColor.y = (detailColor.x * 2.0f) - 1.0f;
    detailColor.z = (detailColor.x * 2.0f) - 1.0f;


    float3 AmbientLight = float3(0.25f, 0.25f, 0.25f) * baseColor.xyz;

    float3 lightDir = -normalize(dir);
    float3 wnrm = detailColor; //normalize(input.norm);

    // Dirctional Light
    float3 directionalLight = saturate((dot(lightDir, wnrm)) * baseColor.xyz * dir_color.xyz);
    float4 dirResult = float4(saturate(AmbientLight + directionalLight), baseColor.w);

    // Point Light
    float3 pointLightDir = normalize(point_pos.xyz - input.worldPos);
    float pointLightRatio = saturate(dot(pointLightDir, wnrm));
    float4 pointResult = pointLightRatio * point_color * baseColor;
    float pointAttenuation = 1.0f - saturate(length(point_pos.xyz - input.worldPos) / point_radius);
    pointAttenuation = pointAttenuation * pointAttenuation;

    // Spot Light
    float3 spotLightDir = normalize(spot_pos.xyz - input.worldPos);
    float spotLightSurfaceRatio = saturate(dot(-spotLightDir, spot_coneDir.xyz));
    float spotLightFactor = (spotLightSurfaceRatio > spot_coneRatio.x) ? 1 : 0;
    float spotLightRatio = saturate(dot(spotLightDir, wnrm));
    float4 spotResult = spotLightFactor * spotLightRatio * spot_color * baseColor;
    float spotAttenuation = saturate((spot_InnerRatio.x - spotLightSurfaceRatio) / (spot_InnerRatio.x - spot_OuterRatio.x));
    spotAttenuation = spotAttenuation * spotAttenuation;

    return saturate((spotResult * spotAttenuation) + (pointResult * pointAttenuation) + dirResult);
}