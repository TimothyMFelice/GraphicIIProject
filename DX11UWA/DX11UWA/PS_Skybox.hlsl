textureCUBE baseTexture : register(t0); // first texture

SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP


// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
    float3 worldPos : WORLD_POS;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    return baseTexture.Sample(filters[1], input.uv);
}
