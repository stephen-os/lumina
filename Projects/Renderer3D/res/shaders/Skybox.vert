#version 330 core

layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoord;

uniform mat4 u_ViewProjection;

void main()
{
    v_TexCoord = a_Position;
    // v_TexCoord = vec3(a_Position.x, -a_Position.y, a_Position.z);
    
    vec4 pos = u_ViewProjection * vec4(a_Position, 1.0);
    
    gl_Position = pos.xyww;
}