// Circle shader for 2D rendering
// Uses SDF (Signed Distance Field) for smooth circles with thickness/fade
// Positions are pre-transformed on CPU

Texture2D u_Texture : register(t0);
SamplerState u_Sampler : register(s0);

struct VSInput
{
    float4 WorldPosition : POSITION;    // Pre-transformed clip-space position
    float4 LocalPosition : LOCALPOS;    // Local coords for SDF (-1 to 1)
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float TexIndex : TEXINDEX;
    float Thickness : THICKNESS;
    float Fade : FADE;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 LocalPosition : LOCALPOS;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    float Thickness : THICKNESS;
    float Fade : FADE;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = input.WorldPosition;
    output.LocalPosition = input.LocalPosition;
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
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
    float4 texColor = u_Texture.Sample(u_Sampler, input.TexCoord);

    // Final color
    float4 color = input.Color * texColor;
    color.a *= circleAlpha;

    if (color.a < 0.001)
        discard;

    return color;
}
