#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

out vec3 v_WorldPos;
out vec4 v_Color;
out vec2 v_TexCoord;
flat out float v_TexIndex;

uniform mat4 u_ViewProjection;
uniform int u_WireframeMode;
uniform vec3 u_WireframeColor;

void main()
{
    v_WorldPos = a_Position;
    v_Color = (u_WireframeMode == 1) ? vec4(u_WireframeColor, 1.0f) : a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}