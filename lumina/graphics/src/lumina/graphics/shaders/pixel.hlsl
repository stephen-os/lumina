// Pixel/Point shader for 2D rendering
// Renders points with variable size
// GPU-accelerated transforms via constant buffer

cbuffer CameraParams : register(b0)
{
    float4x4 u_ViewProjection;
};

struct VSInput
{
    float4 Position : POSITION;     // World position (xyz) + z-index in w
    float4 Color : COLOR;
    float Size : PSIZE;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float Size : PSIZE;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = mul(u_ViewProjection, float4(input.Position.xyz, 1.0));
    output.Position.z = input.Position.w;  // Use w as z-index
    output.Color = input.Color;
    output.Size = input.Size;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.Color;
}
