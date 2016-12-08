// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOut
{
    float4 pos : POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
    float3 worldPos : WORLD_POS;
};

// Simple shader to do vertex processing on the GPU.
VertexShaderOut main(VertexShaderInput input)
{
    // initialixe vetexshaderout structure to all zeros
    VertexShaderOut output = (VertexShaderOut)0;

    // Set out variables to the incoming variables
    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    output.norm = input.norm;

    // Reutrn Out Sturcture
    return output;
}
