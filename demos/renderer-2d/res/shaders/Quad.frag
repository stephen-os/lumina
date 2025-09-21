#version 460 core

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec3 v_WorldPos;
in vec2 v_TexCoord;
flat in float v_TexIndex;

uniform int u_EnableLighting;
uniform vec3 u_AmbientColor;
uniform float u_AmbientIntensity;
uniform int u_PointLightCount;

const int BLEND_ADDITIVE = 0;
const int BLEND_MULTIPLY = 1;
const int BLEND_SCREEN = 2;
const int BLEND_OVERLAY = 3;
const int BLEND_SOFTLIGHT = 4;
const int BLEND_LINEAR_BURN = 5;
const int BLEND_COLOR_DODGE = 6;
const int BLEND_SUBTRACT = 7;
const int BLEND_ALPHA = 8;

const int FALLOFF_NONE = 0;
const int FALLOFF_LINEAR = 1;
const int FALLOFF_QUADRATIC = 2;
const int FALLOFF_INVERSE_SQUARE = 3;
const int FALLOFF_EXPONENTIAL = 4;
const int FALLOFF_SMOOTHSTEP = 5;
const int FALLOFF_CUSTOM = 6;
const int FALLOFF_REALISTIC = 7;

// Point light struct - matches C++ layout (std140 aligned)
struct PointLight
{
    vec3 Position;      // 12 bytes
    float Intensity;    // 4 bytes  (total: 16 bytes - aligned)
    vec3 Color;         // 12 bytes
    float Radius;       // 4 bytes  (total: 16 bytes - aligned)
    float BlendingMode; // 4 bytes
    float BlendAlpha;   // 4 bytes
    float FalloffType;  // 4 bytes
    float Falloff;      // 4 bytes  (total: 16 bytes - aligned)
};                      // Total: 48 bytes per light

layout(std140, binding = 1) uniform PointLights
{
    PointLight u_PointLights[32];
};

uniform int u_WireframeMode;
layout(binding = 0) uniform sampler2D u_Textures[32];

float CalculateAttenuation(float distance, float radius, int falloffType, float falloffParam)
{
    float normalizedDistance = clamp(distance / radius, 0.0, 1.0);
    
    switch (falloffType) 
    {
        case FALLOFF_NONE:
            return 1.0;
        case FALLOFF_LINEAR:
            return max(0.0, 1.0 - normalizedDistance);
        case FALLOFF_QUADRATIC:
            return max(0.0, 1.0 - (normalizedDistance * normalizedDistance));
        case FALLOFF_INVERSE_SQUARE:
            return 1.0 / (1.0 + distance * distance);
        case FALLOFF_EXPONENTIAL:
            return exp(-falloffParam * distance);
        case FALLOFF_SMOOTHSTEP:
            return 1.0 - smoothstep(0.0, 1.0, normalizedDistance);
        case FALLOFF_CUSTOM:
            return pow(max(0.0, 1.0 - normalizedDistance), falloffParam);
        case FALLOFF_REALISTIC:
            return 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        default:
            return 1.0;
    }
}

vec3 ApplyBlendMode(vec3 baseColor, vec3 lightContribution, int blendMode, float alpha)
{
    float lightStrength = length(lightContribution);
    if (lightStrength < 0.001) 
        return baseColor;
    
    vec3 result;
    
    switch (blendMode)
    {
        case BLEND_ADDITIVE:
            result = baseColor + lightContribution;
            break;
            
        case BLEND_MULTIPLY:
            result = baseColor * max(vec3(0.0), vec3(1.0) - lightContribution);
            break;
            
        case BLEND_SCREEN:
            result = vec3(1.0) - (vec3(1.0) - baseColor) * (vec3(1.0) - lightContribution);
            break;
            
        case BLEND_OVERLAY:
            vec3 multiply = 2.0 * baseColor * lightContribution;
            vec3 screen = vec3(1.0) - 2.0 * (vec3(1.0) - baseColor) * (vec3(1.0) - lightContribution);
            result = mix(multiply, screen, step(0.5, baseColor));
            break;
            
        case BLEND_SOFTLIGHT:
            vec3 sqrtBase = sqrt(clamp(baseColor, vec3(0.0), vec3(1.0)));
            vec3 darken = baseColor - (vec3(1.0) - 2.0 * lightContribution) * baseColor * (vec3(1.0) - baseColor);
            vec3 lighten = baseColor + (2.0 * lightContribution - vec3(1.0)) * (sqrtBase - baseColor);
            result = mix(darken, lighten, step(0.5, lightContribution));
            break;
            
        case BLEND_LINEAR_BURN:
            result = max(vec3(0.0), baseColor + lightContribution - vec3(1.0));
            break;
            
        case BLEND_COLOR_DODGE:
            vec3 dodgeLight = clamp(lightContribution, vec3(0.0), vec3(0.999));
            result = clamp(baseColor / max(vec3(0.001), vec3(1.0) - dodgeLight), vec3(0.0), vec3(10.0));
            break;
            
        case BLEND_SUBTRACT:
            result = max(vec3(0.0), baseColor - lightContribution);
            break;
            
        case BLEND_ALPHA:
            result = baseColor + lightContribution;
            break;
            
        default:
            result = baseColor + lightContribution;
            break;
    }
    
    float effectiveAlpha = min(lightStrength, 1.0) * alpha;
    return mix(baseColor, result, effectiveAlpha);
}

vec3 CalculatePointLight(int lightIndex, vec3 worldPos)
{
    PointLight light = u_PointLights[lightIndex];
    
    if (light.Intensity < 0.001)
        return vec3(0.0);
    
    vec3 lightDir = light.Position - worldPos;
    float distance = length(lightDir);
    
    if (distance > light.Radius)
        return vec3(0.0);
    
    if (distance < 0.001)
        distance = 0.001;
    
    lightDir = normalize(lightDir);
    
    float attenuation = CalculateAttenuation(distance, light.Radius, int(light.FalloffType), light.Falloff);
    
    vec3 normal = vec3(0.0, 0.0, 1.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    return light.Color * light.Intensity * attenuation * NdotL;
}

void main()
{
    if (u_WireframeMode == 1)
    {
        o_Color = v_Color; 
        return; 
    }
    
    vec4 texColor = vec4(1.0);
    int index = int(v_TexIndex);
    texColor = texture(u_Textures[index], v_TexCoord);
    
    if (texColor.a < 0.001)
        discard; 
    
    vec4 finalColor = texColor * v_Color;
    
    if (u_EnableLighting == 1)
    {
        finalColor.rgb *= (u_AmbientColor * u_AmbientIntensity);
        
        for (int i = 0; i < u_PointLightCount && i < 32; i++)
        {
            PointLight light = u_PointLights[i];
            
            if (light.Intensity < 0.001)
                continue;
                
            vec3 lightContribution = CalculatePointLight(i, v_WorldPos);
            
            finalColor.rgb = ApplyBlendMode(
                finalColor.rgb, 
                lightContribution, 
                int(light.BlendingMode), 
                light.BlendAlpha
            );
            
            finalColor.rgb = clamp(finalColor.rgb, vec3(0.0), vec3(10.0));
        }
    }
    
    o_Color = vec4(finalColor.rgb, finalColor.a);
}