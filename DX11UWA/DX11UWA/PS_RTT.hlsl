texture2D baseTexture : register(t0); // first texture

SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 baseColor = baseTexture.Sample(filters[0], input.uv);
    return baseColor;
}
