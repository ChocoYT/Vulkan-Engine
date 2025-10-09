struct VertexInput
{
    float3 position : POSITION;
    float3 color    : COLOR;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float3 color    : COLOR;
};

cbuffer CameraBuffer : register(b0)
{
    float4x4 cameraMatrix;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = mul(cameraMatrix, float4(input.position, 1.0));
    output.color    = input.color;

    return output;
}