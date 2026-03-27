// Composite shader for 2D lighting
// Combines the scene texture with the light accumulation buffer
// Final output = scene * (ambient + lights)

Texture2D u_SceneTexture : register(t0);
Texture2D u_LightTexture : register(t1);
SamplerState u_Sampler : register(s0);

cbuffer CompositeParams : register(b0)
{
    float3 u_AmbientColor;
    float u_AmbientIntensity;
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
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // Sample scene and light textures
    float4 sceneColor = u_SceneTexture.Sample(u_Sampler, input.TexCoord);
    float4 lightColor = u_LightTexture.Sample(u_Sampler, input.TexCoord);

    // Calculate total light (ambient + accumulated lights)
    float3 ambient = u_AmbientColor * u_AmbientIntensity;
    float3 totalLight = ambient + lightColor.rgb;

    // Apply lighting to scene (modulate)
    float3 finalColor = sceneColor.rgb * totalLight;

    // Preserve scene alpha
    return float4(finalColor, sceneColor.a);
}
