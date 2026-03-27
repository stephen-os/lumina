// Circle shader for 2D rendering
// Uses SDF (Signed Distance Field) for smooth circles with thickness/fade
// Supports texture arrays (up to 32 textures per batch)
// GPU-accelerated transforms via constant buffer

cbuffer CameraParams : register(b0)
{
    float4x4 u_ViewProjection;
};

Texture2D u_Textures[32] : register(t0);
SamplerState u_Sampler : register(s0);

struct VSInput
{
    float4 WorldPosition : POSITION;    // World position (xyz) + padding
    float4 LocalPosition : LOCALPOS;    // Local coords for SDF (-1 to 1)
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float TexIndex : TEXINDEX;
    float Thickness : THICKNESS;
    float Fade : FADE;
    float ZIndex : ZINDEX;              // Depth within layer (0.0-1.0)
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 LocalPosition : LOCALPOS;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint TexIndex : TEXINDEX;
    float Thickness : THICKNESS;
    float Fade : FADE;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = mul(u_ViewProjection, float4(input.WorldPosition.xyz, 1.0));
    output.Position.z = input.ZIndex;
    output.LocalPosition = input.LocalPosition;
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    output.TexIndex = uint(input.TexIndex);
    output.Thickness = input.Thickness;
    output.Fade = input.Fade;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // Calculate distance from center (local position is -1 to 1)
    float distance = 1.0 - length(input.LocalPosition.xy);

    // Create circle with thickness
    float circleAlpha = smoothstep(0.0, input.Fade, distance);
    circleAlpha *= smoothstep(input.Thickness + input.Fade, input.Thickness, distance);

    // Sample texture
    float4 texColor = u_Textures[input.TexIndex].Sample(u_Sampler, input.TexCoord);

    // Final color
    float4 color = input.Color * texColor;
    color.a *= circleAlpha;

    if (color.a < 0.001)
        discard;

    return color;
}
