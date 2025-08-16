#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

// Instance attributes
layout (location = 5) in mat4 a_InstanceMatrix;         // Takes locations 5, 6, 7, 8
layout (location = 9) in vec4 a_InstanceColor;          // Location 9
layout (location = 10) in float a_InstancePointSize;    // Location 10

uniform mat4 u_ViewProjection;
uniform int u_RenderMode; // 0 = Fill, 1 = Wireframe, 2 = Points
uniform float u_PointSize;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Tangent;
out vec3 v_Bitangent;
out mat3 v_TBN;
out vec4 v_InstanceColor;

void main()
{
    // Use instance matrix for transformation
    vec4 worldPos = a_InstanceMatrix * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    // Transform normal, tangent, and bitangent to world space using instance matrix
    mat3 normalMatrix = transpose(inverse(mat3(a_InstanceMatrix)));
    v_Normal = normalize(normalMatrix * a_Normal);
    v_Tangent = normalize(normalMatrix * a_Tangent);
    v_Bitangent = normalize(normalMatrix * a_Bitangent);
    
    // Create TBN matrix for normal mapping
    v_TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
    
    v_TexCoord = a_TexCoord;
    v_InstanceColor = a_InstanceColor;
    
    // Use instance point size when available, fallback to uniform
    gl_PointSize = a_InstancePointSize > 0.0 ? a_InstancePointSize : u_PointSize;
    
    gl_Position = u_ViewProjection * worldPos;
}