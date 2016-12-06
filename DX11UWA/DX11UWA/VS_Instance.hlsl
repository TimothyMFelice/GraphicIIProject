// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer InstanceModelViewProjectionConstantBuffer : register(b0)
{
    matrix model[5];
    matrix view;
    matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
    float3 worldPos : WORLD_POS;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input, uint id : SV_InstanceID)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
    pos = mul(pos, model[id]);
    output.worldPos = pos;
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;

	// Pass the color through without modification.
    output.uv = input.uv;
    output.norm = mul(input.norm, model[id]);

    return output;
}