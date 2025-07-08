#version 430 core
layout(location = 0) out vec4 o_Color;

in vec3 v_LocalPosition;
in vec4 v_Color;
in vec2 v_TexCoord;
flat in float v_TexIndex;
in float v_Thickness;
in float v_Fade;

uniform int u_WireframeMode;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	if (u_WireframeMode == 1)
	{
		o_Color = v_Color; 
		return; 
	}

    // Calculate distance from center
    float dist = length(v_LocalPosition.xy);
    
    // Simple circle rendering
    if (dist > 1.0) {
        discard;
    }
    
    // Handle thickness (for hollow circles)
    float innerRadius = max(0.0, 1.0 - v_Thickness);
    if (dist < innerRadius) {
        discard;
    }
    
    // Calculate alpha for soft edges
    float alpha = 1.0;
    if (v_Fade > 0.0) {
        // Soft outer edge
        alpha *= 1.0 - smoothstep(1.0 - v_Fade, 1.0, dist);
        // Soft inner edge (for hollow circles)
        if (v_Thickness < 1.0) {
            alpha *= smoothstep(innerRadius, innerRadius + v_Fade, dist);
        }
    }
    
    vec4 texColor = vec4(1.0);
    int texIndex = int(v_TexIndex + 0.5); // Round to nearest int to avoid precision issues
    
    if (texIndex >= 0 && texIndex < 32) {
        texColor = texture(u_Textures[texIndex], v_TexCoord);
    }
    
    o_Color = texColor * v_Color;
    o_Color.a *= alpha;
}