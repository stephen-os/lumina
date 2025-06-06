#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

out vec3 v_Position;
out vec3 v_Normal;
out vec3 v_CameraPosition;
out vec2 v_TexCoord;

uniform mat4 u_Transform;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform vec3 u_CameraPosition;

void main()
{
    mat4 model = u_Transform * u_Model;
    v_Position = vec3(model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(model))) * a_Normal;
    v_CameraPosition = u_CameraPosition;

    // Pass through texture coordinates
    v_TexCoord = a_TexCoord;

    gl_Position = u_Projection * u_View * model * vec4(a_Position, 1.0);
}
