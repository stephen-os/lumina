#version 460 core

out vec4 FragColor;

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

// Basic skybox properties
uniform float u_Intensity;
uniform vec3 u_Tint;

// Advanced properties
uniform float u_Exposure;
uniform float u_Saturation;
uniform float u_Contrast;
uniform float u_Alpha;

// Rotation properties
uniform int u_HasRotation;
uniform mat4 u_RotationMatrix;

// Time of day properties
uniform int u_UseTimeOfDay;
uniform float u_TimeOfDay;

// Render mode
uniform int u_RenderMode = 0;

// Constants
const vec3 GREEN = vec3(0.0, 1.0, 0.3);

// Utility functions
vec3 adjustSaturation(vec3 color, float saturation)
{
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    return mix(vec3(gray), color, saturation);
}

vec3 adjustContrast(vec3 color, float contrast)
{
    return (color - 0.5) * contrast + 0.5;
}

vec3 applyTimeOfDay(vec3 color, float timeOfDay)
{
    // Simple time of day effect - could be much more sophisticated
    // 0.0 = night (dark blue tint), 0.5 = day (normal), 1.0 = night again
    
    float dayStrength = 1.0 - abs(timeOfDay - 0.5) * 2.0; // 0 at night, 1 at noon
    vec3 nightTint = vec3(0.2, 0.3, 0.8); // Blue night tint
    vec3 dayTint = vec3(1.0, 1.0, 1.0);   // Normal day color
    
    vec3 tint = mix(nightTint, dayTint, dayStrength);
    float brightness = 0.3 + 0.7 * dayStrength; // Darker at night
    
    return color * tint * brightness;
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

    vec3 texCoord = v_TexCoord;
    
    // Apply rotation if enabled
    if (u_HasRotation == 1)
    {
        vec4 rotatedCoord = u_RotationMatrix * vec4(texCoord, 1.0);
        texCoord = rotatedCoord.xyz;
    }
    
    // Sample the cubemap
    vec3 color = texture(u_Skybox, texCoord).rgb;
    
    // Apply basic properties
    color *= u_Intensity;
    color *= u_Tint;
    
    // Apply time of day effects
    if (u_UseTimeOfDay == 1)
    {
        color = applyTimeOfDay(color, u_TimeOfDay);
    }
    
    // Apply advanced color grading
    color = adjustSaturation(color, u_Saturation);
    color = adjustContrast(color, u_Contrast);
    
    // Apply exposure (HDR-like effect)
    color = vec3(1.0) - exp(-color * u_Exposure);
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, u_Alpha);
}