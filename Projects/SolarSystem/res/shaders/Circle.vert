#version 430 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in float a_TexIndex;
layout(location = 5) in float a_Thickness;
layout(location = 6) in float a_Fade;

out vec3 v_LocalPosition;
out vec4 v_Color;
out vec2 v_TexCoord;
flat out float v_TexIndex;
out float v_Thickness;
out float v_Fade;

uniform mat4 u_ViewProjection;
uniform int u_WireframeMode;
uniform vec3 u_WireframeColor;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
	v_LocalPosition = a_LocalPosition;
	v_Color = (u_WireframeMode == 1) ? vec4(u_WireframeColor, 1.0f) : a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_Thickness = a_Thickness;
	v_Fade = a_Fade;
}
