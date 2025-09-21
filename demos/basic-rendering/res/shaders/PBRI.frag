#version 330 core

out vec4 FragColor;

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_Tangent;
in vec3 v_Bitangent;
in mat3 v_TBN;
in vec4 v_InstanceColor;

struct Material 
{
    vec3 Albedo;
    float Metallic;
    float Roughness;
    float AO;
    vec3 Emission;
    float EmissionIntensity;
    float HeightScale;
    float Alpha;
    float NormalIntensity;
};

uniform Material u_Material;
uniform vec4 u_TintColor;
uniform int u_RenderMode; // 0 = Fill, 1 = Wireframe, 2 = Points

// Texture uniforms - all 8 material slots
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_RoughnessTexture;
uniform sampler2D u_AOTexture;
uniform sampler2D u_EmissionTexture;
uniform sampler2D u_HeightTexture;
uniform sampler2D u_AlphaTexture;
uniform samplerCube u_EnvironmentMap;

uniform int u_HasAlbedoTexture;
uniform int u_HasNormalTexture;
uniform int u_HasMetallicTexture;
uniform int u_HasRoughnessTexture;
uniform int u_HasAOTexture;
uniform int u_HasEmissionTexture;
uniform int u_HasHeightTexture;
uniform int u_HasAlphaTexture;
uniform int u_HasEnvironmentMap;

// Lighting uniforms
struct DirectionalLight 
{
    vec3 Direction;
    vec3 Color;
    float Intensity;
};

struct PointLight 
{
    vec3 Position;
    vec3 Color;
    float Intensity;
    float Constant;
    float Linear;
    float Quadratic;
};

uniform DirectionalLight u_DirLight;
uniform PointLight u_PointLights[32];
uniform int u_NumPointLights;

uniform vec3 u_CameraPos;

// Constants
const float PI = 3.14159265359;
const vec3 GREEN = vec3(0.0, 1.0, 0.3);

// Enhanced Functions
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
    if (u_HasHeightTexture == 0) return texCoords;
    
    // Simple parallax mapping
    float height = texture(u_HeightTexture, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * u_Material.HeightScale);
    return texCoords - p;
}

vec3 getNormalFromMap(vec2 texCoords)
{
    if (u_HasNormalTexture == 1 && u_RenderMode != 2) // Don't use normal maps for points
    {
        vec3 tangentNormal = texture(u_NormalTexture, texCoords).xyz * 2.0 - 1.0;
        
        // Apply normal intensity
        tangentNormal.xy *= u_Material.NormalIntensity;
        tangentNormal = normalize(tangentNormal);
        
        return normalize(v_TBN * tangentNormal);
    }
    else
    {
        return normalize(v_Normal);
    }
}

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // Handle point rendering
    if (u_RenderMode == 2) // Points mode
    {
        FragColor = vec4(GREEN, 1.0f); 
        return;
    }
    
    // Handle wireframe rendering
    if (u_RenderMode == 1) // Wireframe mode
    {   
        FragColor = vec4(GREEN, 1.0f);
        return;
    }
    
    // Standard PBR rendering for Fill mode
    vec3 V = normalize(u_CameraPos - v_WorldPos);
    
    // Apply parallax mapping if height texture is available
    vec2 texCoords = v_TexCoord;
    if (u_HasHeightTexture == 1)
    {
        vec3 viewDirTangent = normalize(transpose(v_TBN) * V);
        texCoords = parallaxMapping(v_TexCoord, viewDirTangent);
        
        // Discard fragments outside [0,1] range to avoid texture wrapping artifacts
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
    }
    
    // Sample material properties with updated texture coordinates
    vec3 albedo = u_Material.Albedo;
    if (u_HasAlbedoTexture == 1)
        albedo *= texture(u_AlbedoTexture, texCoords).rgb;
    
    // Use instance color if available, otherwise use uniform tint color
    vec3 tintColor = v_InstanceColor.a > 0.0 ? v_InstanceColor.rgb : u_TintColor.rgb;
    albedo *= tintColor;
    
    float metallic = u_Material.Metallic;
    if (u_HasMetallicTexture == 1)
        metallic *= texture(u_MetallicTexture, texCoords).r;
    
    float roughness = u_Material.Roughness;
    if (u_HasRoughnessTexture == 1)
        roughness *= texture(u_RoughnessTexture, texCoords).r;
    
    float ao = u_Material.AO;
    if (u_HasAOTexture == 1)
        ao *= texture(u_AOTexture, texCoords).r;

    // Sample emission
    vec3 emission = u_Material.Emission * u_Material.EmissionIntensity;
    if (u_HasEmissionTexture == 1)
        emission *= texture(u_EmissionTexture, texCoords).rgb;

    // Sample alpha
    float alpha = u_Material.Alpha;
    if (u_HasAlphaTexture == 1)
        alpha *= texture(u_AlphaTexture, texCoords).r;

    // Use instance alpha if available, otherwise use uniform tint alpha
    float finalAlpha = v_InstanceColor.a > 0.0 ? v_InstanceColor.a : u_TintColor.a;
    alpha *= finalAlpha;

    // Early discard for fully transparent fragments
    if (alpha < 0.01)
        discard;

    vec3 N = getNormalFromMap(texCoords);

    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    
    // Directional light
    {
        vec3 L = normalize(-u_DirLight.Direction);
        vec3 H = normalize(V + L);
        float distance = 1.0;
        float attenuation = 1.0;
        vec3 radiance = u_DirLight.Color * u_DirLight.Intensity * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    // Point lights
    for(int i = 0; i < u_NumPointLights && i < 32; ++i)
    {
        vec3 L = normalize(u_PointLights[i].Position - v_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(u_PointLights[i].Position - v_WorldPos);
        float attenuation = 1.0 / (u_PointLights[i].Constant + 
                                   u_PointLights[i].Linear * distance + 
                                   u_PointLights[i].Quadratic * (distance * distance));
        vec3 radiance = u_PointLights[i].Color * u_PointLights[i].Intensity * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient lighting (simple implementation)
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    // Environment reflection (simple implementation)
    if (u_HasEnvironmentMap == 1)
    {
        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        
        vec3 R = reflect(-V, N);
        vec3 envColor = texture(u_EnvironmentMap, R).rgb;
        vec3 envReflection = envColor * (F * 0.1); // Simple environment reflection
        ambient += envReflection;
    }

    vec3 color = ambient + Lo;

    // Add emission (emissive materials glow regardless of lighting)
    color += emission;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, alpha);
}