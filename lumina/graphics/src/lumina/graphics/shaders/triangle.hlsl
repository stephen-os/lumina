// Triangle shader for 2D rendering
// Simple textured triangles with vertex colors
// Positions are pre-transformed on CPU

Texture2D u_Texture : register(t0);
SamplerState u_Sampler : register(s0);

struct VSInput
{
    float4 Position : POSITION;     // Pre-transformed clip-space position
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float TexIndex : TEXINDEX;
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
    output.Position = input.Position;
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = u_Texture.Sample(u_Sampler, input.TexCoord);
    return input.Color * texColor;
}
