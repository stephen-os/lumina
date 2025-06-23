#version 450 core

in vec4 v_Color;
in vec2 v_TexCoord;
flat in float v_TexIndex;

out vec4 color;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
    vec4 texColor = vec4(1.0);
    
    // Determine which texture to sample based on v_TexIndex
    int index = int(v_TexIndex);
    
    texColor = texture(u_Textures[index], v_TexCoord);

    // Discard fragment if alpha is 0
    if (texColor.a < 0.001)
        discard; 

    // Apply tint color
    color = texColor * v_Color;
}