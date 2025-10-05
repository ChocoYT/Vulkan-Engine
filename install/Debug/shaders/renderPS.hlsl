struct PixelInputType
{
    float4 position : SV_Position;
};

struct PixelOutput
{
    float4 color : SV_Target;
};

PixelOutput main()
{
    PixelOutput output;
    output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}
