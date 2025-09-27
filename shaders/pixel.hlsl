struct PixelOutput {
    float4 color : SV_Target;
};

PixelOutput PSMain()
{
    PixelOutput output;
    output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}
