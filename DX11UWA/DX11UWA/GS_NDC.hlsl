// Input structure
struct GSInput
{
    float3 pos : POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
};

// Output structure
struct GSOutput
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
    float3 norm : NORMAL;
    float3 worldPos : WORLD_POS;
    float4 tangent : TANGENT;
    float4 biTangent : BI_TANGENT;
    float4 campPos : CAM_POS;
};

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer InstanceModelViewProjectionConstantBuffer : register(b0)
{
    matrix model[5];
    matrix view;
    matrix projection;
    float4 camPos;
};

// number of vertices this geometry shader will send to the rasterizer
[maxvertexcount(24)]
// instance things
[instance(5)]
// main function for the geometry shader
// line input[2]. triangle Input[3]
void main(point GSInput input[1], inout TriangleStream< GSOutput > output, uint id : SV_GSInstanceID)
{
    GSOutput element[24];
    for (int t = 0; t < 24; ++t)
        element[t] = (GSOutput) 0;

    for (int i = 0; i < 24; ++i)
    {
        element[i].pos = float4(input[0].pos, 1.0f);
        element[i].campPos = camPos;
    }


    float size = 0.125f;
    // FRONT FACE
    element[0].pos.x -= size;
    element[0].pos.y += size;
    element[0].pos.z -= size;

    element[1].pos.x += size;
    element[1].pos.y += size;
    element[1].pos.z -= size;
    
    element[2].pos.x -= size;
    element[2].pos.y -= size;
    element[2].pos.z -= size;
    
    element[3].pos.x += size;
    element[3].pos.y -= size;
    element[3].pos.z -= size;

    element[0].uv = float3(0.0f, 0.0f, 3.0f);
    element[1].uv = float3(1.0f, 0.0f, 3.0f);
    element[2].uv = float3(0.0f, 1.0f, 3.0f);
    element[3].uv = float3(1.0f, 1.0f, 3.0f);

    // RIGHT FACE
    element[4].pos.x += size;
    element[4].pos.y += size;
    element[4].pos.z -= size;

    element[5].pos.x += size;
    element[5].pos.y += size;
    element[5].pos.z += size;
    
    element[6].pos.x += size;
    element[6].pos.y -= size;
    element[6].pos.z -= size;
    
    element[7].pos.x += size;
    element[7].pos.y -= size;
    element[7].pos.z += size;

    element[4].uv = float3(0.0f, 0.0f, 3.0f);
    element[5].uv = float3(1.0f, 0.0f, 3.0f);
    element[6].uv = float3(0.0f, 1.0f, 3.0f);
    element[7].uv = float3(1.0f, 1.0f, 3.0f);

    // BACK FACE
    element[8].pos.x += size;
    element[8].pos.y += size;
    element[8].pos.z += size;

    element[9].pos.x -= size;
    element[9].pos.y += size;
    element[9].pos.z += size;
    
    element[10].pos.x += size;
    element[10].pos.y -= size;
    element[10].pos.z += size;
    
    element[11].pos.x -= size;
    element[11].pos.y -= size;
    element[11].pos.z += size;

    element[8].uv = float3(0.0f, 0.0f, 3.0f);
    element[9].uv = float3(1.0f, 0.0f, 3.0f);
    element[10].uv = float3(0.0f, 1.0f, 3.0f);
    element[11].uv = float3(1.0f, 1.0f, 3.0f);
    
    // LEFT FACE
    element[12].pos.x -= size;
    element[12].pos.y += size;
    element[12].pos.z += size;

    element[13].pos.x -= size;
    element[13].pos.y += size;
    element[13].pos.z -= size;
    
    element[14].pos.x -= size;
    element[14].pos.y -= size;
    element[14].pos.z += size;
    
    element[15].pos.x -= size;
    element[15].pos.y -= size;
    element[15].pos.z -= size;

    element[12].uv = float3(0.0f, 0.0f, 3.0f);
    element[13].uv = float3(1.0f, 0.0f, 3.0f);
    element[14].uv = float3(0.0f, 1.0f, 3.0f);
    element[15].uv = float3(1.0f, 1.0f, 3.0f);

    // TOP FACE
    element[16].pos.x -= size;
    element[16].pos.y += size;
    element[16].pos.z += size;

    element[17].pos.x += size;
    element[17].pos.y += size;
    element[17].pos.z += size;
    
    element[18].pos.x -= size;
    element[18].pos.y += size;
    element[18].pos.z -= size;
    
    element[19].pos.x += size;
    element[19].pos.y += size;
    element[19].pos.z -= size;

    element[16].uv = float3(0.0f, 0.0f, 3.0f);
    element[17].uv = float3(1.0f, 0.0f, 3.0f);
    element[18].uv = float3(0.0f, 1.0f, 3.0f);
    element[19].uv = float3(1.0f, 1.0f, 3.0f);

    // BOT FACE
    element[20].pos.x -= size;
    element[20].pos.y -= size;
    element[20].pos.z -= size;

    element[21].pos.x += size;
    element[21].pos.y -= size;
    element[21].pos.z -= size;
    
    element[22].pos.x -= size;
    element[22].pos.y -= size;
    element[22].pos.z += size;
    
    element[23].pos.x += size;
    element[23].pos.y -= size;
    element[23].pos.z += size;

    element[20].uv = float3(0.0f, 0.0f, 3.0f);
    element[21].uv = float3(1.0f, 0.0f, 3.0f);
    element[22].uv = float3(0.0f, 1.0f, 3.0f);
    element[23].uv = float3(1.0f, 1.0f, 3.0f);

    // IF you want to transform these itmes into 3d space
    for (int k = 0; k < 24; ++k)
    {
        float4 tmp = float4(element[k].pos);
        tmp = mul(tmp, model[id]);
        //element[k].worldPos = tmp;
        tmp = mul(tmp, view);
        tmp = mul(tmp, projection);

        element[k].pos = tmp;
    }

    //FRONT
    // Appending to the output container the vertices we created inside the geometry shader
    for (int a = 0; a < 4; ++a)
    {
        element[a].norm = float3(0.0f, 0.0f, -1.0f);
        output.Append(element[a]);
    }

    output.RestartStrip();

    //RIGHT
    // Appending to the output container the vertices we created inside the geometry shader
    for (int b = 4; b < 8; ++b)
    {
        element[b].norm = float3(1.0f, 0.0f, 0.0f);
        output.Append(element[b]);
    }

    output.RestartStrip();

    //BACK
    // Appending to the output container the vertices we created inside the geometry shader
    for (int c = 8; c < 12; ++c)
    {
        element[c].norm = float3(0.0f, 0.0f, 1.0f);
        output.Append(element[c]);
    }

    output.RestartStrip();

    //LEFT
    // Appending to the output container the vertices we created inside the geometry shader
    for (int d = 12; d < 16; ++d)
    {
        element[d].norm = float3(-1.0f, 0.0f, 0.0f);
        output.Append(element[d]);
    }

    output.RestartStrip();

    //TOP
    // Appending to the output container the vertices we created inside the geometry shader
    for (int e = 16; e < 20; ++e)
    {
        element[e].norm = float3(0.0f, 1.0f, 0.0f);
        output.Append(element[e]);
    }

    output.RestartStrip(); 
    
    //BOT
    // Appending to the output container the vertices we created inside the geometry shader
    for (int f = 20; f < 24; ++f)
    {
        element[f].norm = float3(0.0f, -1.0f, 0.0f);
        output.Append(element[f]);
    }
}