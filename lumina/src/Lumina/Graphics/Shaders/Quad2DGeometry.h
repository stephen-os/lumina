#pragma once

namespace Lumina
{
    namespace Shaders
    {
        namespace Quad2DGeometry
        {
            const char* Vertex =
                R"(
                #version 460 core

                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;
                layout(location = 2) in vec2 a_TexCoord;
                layout(location = 3) in float a_TexIndex;

                out vec4 v_Color;
                out vec3 v_WorldPos;
                out vec2 v_TexCoord;
                flat out float v_TexIndex;

                uniform mat4 u_ViewProjection;

                void main()
                {
                    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
                    v_Color = a_Color;
                    v_TexCoord = a_TexCoord;
                    v_WorldPos = a_Position;
                    v_TexIndex = a_TexIndex;
                }
            )";

            const char* Fragment =
                R"(
                #version 460 core

                // G-Buffer outputs
                layout(location = 0) out vec4 o_Position;  // World position
                layout(location = 1) out vec4 o_Color;     // Albedo color
                layout(location = 2) out vec4 o_Normal;    // Normal (always 0,0,1 for 2D)

                in vec4 v_Color;
                in vec3 v_WorldPos;
                in vec2 v_TexCoord;
                flat in float v_TexIndex;

                layout(binding = 0) uniform sampler2D u_Textures[32];

                void main()
                {
                    vec4 texColor = vec4(1.0);
                    int index = int(v_TexIndex);
                    texColor = texture(u_Textures[index], v_TexCoord);

                    if (texColor.a < 0.001)
                        discard;

                    vec4 finalColor = texColor * v_Color;

                    // Write to G-Buffer
                    o_Position = vec4(v_WorldPos, 1.0);
                    o_Color = finalColor;
                    o_Normal = vec4(0.0, 0.0, 1.0, 1.0);  // 2D normal
                }
            )";
        }
    }
}