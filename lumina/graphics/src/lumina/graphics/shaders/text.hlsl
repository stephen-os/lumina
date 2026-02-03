// Text shader for 2D rendering
// Uses font texture atlas (16x16 grid of ASCII characters)
// Positions are pre-transformed on CPU

Texture2D u_FontTexture : register(t0);
SamplerState u_Sampler : register(s0);

struct VSInput
{
    float4 Position : POSITION;     // Pre-transformed clip-space position
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float TexIndex : TEXINDEX;      // Reserved for multi-font support
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
    // Sample font texture (typically grayscale alpha)
    float4 texColor = u_FontTexture.Sample(u_Sampler, input.TexCoord);

    // Use texture alpha for text rendering
    float4 color = input.Color;
    color.a *= texColor.a;

    if (color.a < 0.001)
        discard;

    return color;
}
