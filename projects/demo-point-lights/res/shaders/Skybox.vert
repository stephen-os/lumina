#version 450 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;

// Optional rotation matrix for skybox
uniform int u_HasRotation;
uniform mat4 u_RotationMatrix;

out vec3 v_TexCoord;

void main()
{
    // Use position as texture coordinate for cubemap sampling
    v_TexCoord = a_Position;
    
    // Apply optional rotation to the position for animated skyboxes
    vec3 position = a_Position;
    if (u_HasRotation == 1)
    {
        vec4 rotatedPos = u_RotationMatrix * vec4(a_Position, 1.0);
        position = rotatedPos.xyz;
    }
    
    // Transform to clip space
    // Note: We use the original a_Position for rendering, but rotated position for texture coords
    vec4 pos = u_ViewProjection * vec4(a_Position, 1.0);
    gl_Position = pos.xyww; // Set z to w for depth testing (always farthest)
}