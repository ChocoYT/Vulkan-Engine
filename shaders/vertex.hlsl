struct VertexOutput {
    float4 sv_position : SV_Position;
};

static const float2 positions[3] = {
    float2(0.0f, -0.5f),
    float2(0.5f, 0.5f),
    float2(-0.5f, 0.5f)
};

VertexOutput VSMain(uint vid : SV_VertexID)
{
    VertexOutput output;
    output.sv_position = float4(positions[vid], 0.0f, 1.0f);

    return output;
}
