struct PixelInputType
{
    float4 position : SV_Position;
    float3 color    : COLOR;
};

struct PixelOutput
{
    float4 color : SV_Target;
};

PixelOutput main(PixelInputType input)
{
    PixelOutput output;
    output.color = float4(input.color, 1.0);
    
    return output;
}
