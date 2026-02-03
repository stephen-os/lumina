// Quad shader for 2D rendering
// Simple single-texture version for testing
// Positions are pre-transformed on CPU (no constant buffer needed)

Texture2D u_Texture : register(t0);
SamplerState u_Sampler : register(s0);

struct VSInput
{
    float4 Position : POSITION;  // Pre-transformed clip-space position
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float TexIndex : TEXINDEX;   // Ignored for now
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = input.Position;  // Already in clip space
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = u_Texture.Sample(u_Sampler, input.TexCoord);
    return input.Color * texColor;
}
