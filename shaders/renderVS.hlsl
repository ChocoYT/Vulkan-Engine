struct VertexOutput
{
    float4 position : SV_Position;
};

static const float2 positions[3] = {
    float2(0.0f, -0.5f),
    float2(0.5f, 0.5f),
    float2(-0.5f, 0.5f)
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.position = float4(positions[vertexID], 0.0f, 1.0f);

    return output;
}
