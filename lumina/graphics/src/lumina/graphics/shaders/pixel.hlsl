// Pixel/Point shader for 2D rendering
// Renders points with variable size
// Positions are pre-transformed on CPU

struct VSInput
{
    float4 Position : POSITION;     // Pre-transformed clip-space position
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
    output.Position = input.Position;
    output.Color = input.Color;
    output.Size = input.Size;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.Color;
}
