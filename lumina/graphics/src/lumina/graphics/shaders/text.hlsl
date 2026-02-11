// Text shader for 2D rendering
// Uses font texture atlas with multi-font support via texture arrays
// GPU-accelerated transforms via constant buffer

cbuffer CameraParams : register(b0)
{
    float4x4 u_ViewProjection;
};

Texture2D u_Textures[32] : register(t0);
SamplerState u_Sampler : register(s0);

struct VSInput
{
    float4 Position : POSITION;     // World position (xyz) + padding
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float TexIndex : TEXINDEX;      // Font texture index
    float ZIndex : ZINDEX;          // Depth within layer (0.0-1.0)
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint TexIndex : TEXINDEX;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = mul(u_ViewProjection, float4(input.Position.xyz, 1.0));
    output.Position.z = input.ZIndex;
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    output.TexIndex = uint(input.TexIndex);
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // Sample font texture (typically grayscale alpha)
    float4 texColor = u_Textures[input.TexIndex].Sample(u_Sampler, input.TexCoord);

    // Use texture alpha for text rendering
    float4 color = input.Color;
    color.a *= texColor.a;

    if (color.a < 0.001)
        discard;

    return color;
}
