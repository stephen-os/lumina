#version 330 core

in vec3 v_TexCoord;

out vec4 FragColor;

uniform samplerCube u_Skybox;
uniform float u_Intensity;
uniform vec3 u_Tint;

void main()
{
    vec3 color = texture(u_Skybox, v_TexCoord).rgb;
    
    // Apply intensity scaling
    color *= u_Intensity;
    
    // Apply tint color
    color *= u_Tint;
    
    FragColor = vec4(color, 1.0);
}