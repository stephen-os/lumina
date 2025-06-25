#version 430 core

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in float v_TexIndex;

uniform int u_WireframeMode;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	if (u_WireframeMode == 1)
	{
		o_Color = v_Color; 
		return; 
	}

	vec4 texColor = vec4(1.0);
    
	int index = int(v_TexIndex);
    
	texColor = texture(u_Textures[index], v_TexCoord);

	if (texColor.a < 0.001)
		discard; 

	o_Color = texColor * v_Color;
}