#pragma once

namespace Lumina
{
	namespace Shaders
	{
		namespace Grid2D
		{
			constexpr const char* Vertex = 
			R"(
				#version 460 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;
				layout(location = 2) in vec2 a_TexCoord;
				layout(location = 3) in float a_TexIndex;
				layout(location = 4) in vec3 a_GridPosition;
				layout(location = 5) in vec2 a_GridSize;
				layout(location = 6) in float a_CellSize;
				layout(location = 7) in vec4 a_GridColor;
				layout(location = 8) in float a_LineWidth;
				layout(location = 9) in float a_ShowCheckerboard;
				layout(location = 10) in vec4 a_CheckerColor1;
				layout(location = 11) in vec4 a_CheckerColor2;

				layout(location = 0) uniform mat4 u_ViewProjection;

				layout(location = 0) out vec3 v_WorldPosition;
				layout(location = 1) out vec2 v_TexCoord;
				layout(location = 2) out vec4 v_Color;
				layout(location = 3) out vec3 v_GridPosition;
				layout(location = 4) out vec2 v_GridSize;
				layout(location = 5) out float v_CellSize;
				layout(location = 6) out vec4 v_GridColor;
				layout(location = 7) out float v_LineWidth;
				layout(location = 8) out float v_ShowCheckerboard;
				layout(location = 9) out vec4 v_CheckerColor1;
				layout(location = 10) out vec4 v_CheckerColor2;

				void main()
				{
					v_WorldPosition = a_Position;
					v_TexCoord = a_TexCoord;
					v_Color = a_Color;
					v_GridPosition = a_GridPosition;
					v_GridSize = a_GridSize;
					v_CellSize = a_CellSize;
					v_GridColor = a_GridColor;
					v_LineWidth = a_LineWidth;
					v_ShowCheckerboard = a_ShowCheckerboard;
					v_CheckerColor1 = a_CheckerColor1;
					v_CheckerColor2 = a_CheckerColor2;
    
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				}	
			)";

			constexpr const char* Fragment =
			R"(
				#version 460 core

				layout(location = 0) in vec3 v_WorldPosition;
				layout(location = 1) in vec2 v_TexCoord;
				layout(location = 2) in vec4 v_Color;
				layout(location = 3) in vec3 v_GridPosition;
				layout(location = 4) in vec2 v_GridSize;
				layout(location = 5) in float v_CellSize;
				layout(location = 6) in vec4 v_GridColor;
				layout(location = 7) in float v_LineWidth;
				layout(location = 8) in float v_ShowCheckerboard;
				layout(location = 9) in vec4 v_CheckerColor1;
				layout(location = 10) in vec4 v_CheckerColor2;

				layout(location = 0) out vec4 o_Color;

				void main()
				{
					const vec2 worldPos = v_WorldPosition.xy - v_GridPosition.xy;
					const vec2 gridPos = worldPos + v_GridSize * 0.5f;    
					const vec2 checkerGroupCoord = gridPos / (v_CellSize * 2.0f);
					const vec2 groupLocalPos = fract(checkerGroupCoord); 
					const ivec2 checkerCell = ivec2(floor(checkerGroupCoord * 2.0f));
    
					vec4 finalColor = vec4(0.0f);

					if (v_ShowCheckerboard > 0.5f) 
					{
						const bool isEvenChecker = ((checkerCell.x + checkerCell.y) & 1) == 0;
						const vec4 checkerColor = isEvenChecker ? v_CheckerColor1 : v_CheckerColor2;
						finalColor = mix(finalColor, checkerColor, checkerColor.a);
					}
    
					const vec2 pixelSize = fwidth(checkerGroupCoord);
					const float lineWidth = v_LineWidth * max(pixelSize.x, pixelSize.y);

					const vec2 distToGroupEdge = min(groupLocalPos, 1.0f - groupLocalPos);
					const float minDistToEdge = min(distToGroupEdge.x, distToGroupEdge.y);

					const float gridAlpha = 1.0f - smoothstep(0.0f, lineWidth, minDistToEdge);

					finalColor = mix(finalColor, v_GridColor, gridAlpha * v_GridColor.a);
    
					if (finalColor.a < 0.01f)
						discard;
    
					o_Color = finalColor;
				}
			)";
		}
	}
}