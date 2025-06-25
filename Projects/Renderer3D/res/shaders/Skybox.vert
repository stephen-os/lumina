#version 330 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;

out vec3 v_TexCoord;

void main()
{
    v_TexCoord = a_Position;
    
    // Remove translation from view matrix for skybox effect
    mat4 view = mat4(mat3(u_ViewProjection));
    vec4 pos = view * vec4(a_Position, 1.0);
    
    // Set z to w so that after perspective divide, z will be 1.0 (maximum depth)
    gl_Position = pos.xyww;
}