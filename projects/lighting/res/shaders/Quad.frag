#version 460 core

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec3 v_WorldPos;
in vec2 v_TexCoord;
flat in float v_TexIndex;

// Lighting uniforms
uniform int u_EnableLighting;
uniform vec3 u_AmbientColor;
uniform float u_AmbientIntensity;
uniform int u_PointLightCount;

// Define enums using const int (GLSL doesn't have enums)
const int BLEND_ADDITIVE = 0;
const int BLEND_MULTIPLY = 1;

const int FALLOFF_LINEAR = 0;
const int FALLOFF_QUADRATIC = 1;
const int FALLOFF_CUSTOM = 2;

// Point light struct - matches C++ layout
struct PointLight
{
    vec3 Position;
    float Intensity;
    vec3 Color;
    float Radius;
    float BlendingMode;
    float Falloff;
    float Unused0;
    float Unused1;
};

// Point light uniform buffer
layout(std140, binding = 1) uniform PointLights
{
    PointLight u_PointLights[32];
};

// Texture and wireframe uniforms
uniform int u_WireframeMode;
layout(binding = 0) uniform sampler2D u_Textures[32];

float CalculateAttenuation(float distance, float radius, int falloffType)
{
    float normalizedDistance = clamp(distance / radius, 0.0, 1.0);
    
    if (falloffType == FALLOFF_LINEAR) {
        // Linear falloff
        return 1.0 - normalizedDistance;
    } else if (falloffType == FALLOFF_QUADRATIC) {
        // Quadratic falloff (physically accurate)
        return 1.0 - (normalizedDistance * normalizedDistance);
    } else if (falloffType == FALLOFF_CUSTOM) {
        // Custom power falloff - using Unused0 as exponent
        float exponent = u_PointLights[0].Unused0; // You'd need to pass this per light
        return 1.0 - pow(normalizedDistance, exponent);
    }
    
    return 1.0 - normalizedDistance; // Fallback to linear
}

vec3 ApplyBlendMode(vec3 baseColor, vec3 lightContribution, int blendMode)
{
    if (blendMode == BLEND_ADDITIVE) {
        // Additive - standard lighting
        return baseColor + lightContribution;
    } else if (blendMode == BLEND_MULTIPLY) {
        // Multiply - darkening effect (good for shadows)
        return baseColor * (vec3(1.0) - lightContribution);
    }
    
    return baseColor; // Fallback
}

vec3 CalculatePointLight(int lightIndex, vec3 worldPos)
{
    PointLight light = u_PointLights[lightIndex];
    
    vec3 lightDir = light.Position - worldPos;
    float distance = length(lightDir);
    
    // Early exit if outside light radius
    if (distance > light.Radius)
        return vec3(0.0);
    
    // Normalize light direction
    lightDir = normalize(lightDir);
    
    // Calculate attenuation using falloff type
    int falloffType = int(light.Falloff);
    float attenuation = CalculateAttenuation(distance, light.Radius, falloffType);
    
    // Simple diffuse lighting (assuming normal pointing up for 2D)
    vec3 normal = vec3(0.0, 0.0, 1.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Calculate light contribution
    vec3 lightContribution = light.Color * light.Intensity * attenuation * NdotL;
    
    return lightContribution;
}

void main()
{
    // Handle wireframe mode first
    if (u_WireframeMode == 1)
    {
        o_Color = v_Color; 
        return; 
    }
    
    // Sample texture
    vec4 texColor = vec4(1.0);
    int index = int(v_TexIndex);
    texColor = texture(u_Textures[index], v_TexCoord);
    
    if (texColor.a < 0.001)
        discard; 
    
    // Apply vertex color
    vec4 finalColor = texColor * v_Color;
    
    // Apply lighting if enabled
    if (u_EnableLighting == 1)
    {
        // Start with ambient lighting
        vec3 ambient = u_AmbientColor * u_AmbientIntensity;
        vec3 lighting = ambient;
        
        // Add point light contributions
        for (int i = 0; i < u_PointLightCount && i < 32; i++)
        {
            vec3 lightContribution = CalculatePointLight(i, v_WorldPos);
            
            // Apply blending mode for this light
            PointLight light = u_PointLights[i];
            int blendMode = int(light.BlendingMode);
            lighting = ApplyBlendMode(lighting, lightContribution, blendMode);
        }
        
        // Apply final lighting to color
        finalColor.rgb *= lighting;
    }
    
    o_Color = finalColor;
}