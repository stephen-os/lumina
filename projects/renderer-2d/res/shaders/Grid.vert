#version 450 core

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