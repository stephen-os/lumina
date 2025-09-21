#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform int u_RenderMode;
uniform float u_PointSize;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Tangent;
out vec3 v_Bitangent;
out mat3 v_TBN;

void main()
{
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    // Transform normal, tangent, and bitangent to world space
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    v_Normal = normalize(normalMatrix * a_Normal);
    v_Tangent = normalize(normalMatrix * a_Tangent);
    v_Bitangent = normalize(normalMatrix * a_Bitangent);
    
    // Create TBN matrix for normal mapping
    v_TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
    
    v_TexCoord = a_TexCoord;
    
    // Always set point size (OpenGL needs this for point rendering to work)
    gl_PointSize = u_PointSize;
    
    gl_Position = u_ViewProjection * worldPos;
}