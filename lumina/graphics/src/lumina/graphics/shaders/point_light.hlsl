// Point Light shader for 2D lighting
// Renders a point light with configurable attenuation
// Used with additive blending into the light accumulation buffer

cbuffer LightParams : register(b0)
{
    float2 u_LightPosition;     // Light position in world space
    float2 u_ViewportSize;      // Viewport dimensions for screen-to-world conversion
    float3 u_LightColor;
    float u_LightIntensity;
    float u_LightRadius;
    float u_AttenuationModel;   // 0=none, 1=linear, 2=quadratic, 3=inverse_square, 4=exponential, 5=smoothstep, 6=realistic
    float u_Falloff;
    float _pad;
    float4x4 u_InverseViewProj; // To convert screen position to world position
};

struct VSInput
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float2 ScreenPos : SCREENPOS;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    // Screen position in pixels
    output.ScreenPos = (input.TexCoord * float2(2.0, -2.0) + float2(-1.0, 1.0)) * u_ViewportSize * 0.5 + u_ViewportSize * 0.5;
    return output;
}

float calculate_attenuation(float distance, float radius, float model, float falloff)
{
    if (distance >= radius)
        return 0.0;

    float normalized = distance / radius;

    // Attenuation models matching the enum
    if (model < 0.5) // none
    {
        return 1.0;
    }
    else if (model < 1.5) // linear
    {
        return 1.0 - normalized;
    }
    else if (model < 2.5) // quadratic
    {
        float t = 1.0 - normalized;
        return t * t;
    }
    else if (model < 3.5) // inverse_square
    {
        float d = max(distance, 0.001);
        return radius * radius / (d * d + 1.0);
    }
    else if (model < 4.5) // exponential
    {
        return exp(-falloff * normalized);
    }
    else if (model < 5.5) // smoothstep
    {
        return smoothstep(1.0, 0.0, normalized);
    }
    else // realistic (physically-based)
    {
        float d = max(distance, 0.001);
        float attenuation = 1.0 / (1.0 + (d / radius) * (d / radius));
        // Smooth cutoff at radius
        float cutoff = saturate(1.0 - (distance / radius));
        return attenuation * cutoff * cutoff;
    }
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // Convert screen position back to world position using inverse view-projection
    float2 ndc = input.TexCoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
    float4 worldPos = mul(u_InverseViewProj, float4(ndc, 0.0, 1.0));
    worldPos /= worldPos.w;

    // Calculate distance to light in world space
    float2 toLight = u_LightPosition - worldPos.xy;
    float distance = length(toLight);

    // Calculate attenuation
    float attenuation = calculate_attenuation(distance, u_LightRadius, u_AttenuationModel, u_Falloff);

    // Final light contribution
    float3 lightContrib = u_LightColor * u_LightIntensity * attenuation;

    return float4(lightContrib, 1.0);
}
