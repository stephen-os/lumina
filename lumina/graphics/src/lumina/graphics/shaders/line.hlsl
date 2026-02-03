// Line shader for 2D rendering
// Simple vertex color passthrough
// Positions are pre-transformed on CPU (no constant buffer needed)

struct VSInput
{
    float4 Position : POSITION;  // Pre-transformed clip-space position
    float4 Color : COLOR;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = input.Position;  // Already in clip space
    output.Color = input.Color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.Color;
}
