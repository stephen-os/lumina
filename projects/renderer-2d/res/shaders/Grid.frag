#version 450 core

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

layout(location = 0) out vec4 FragColor;

void main()
{
    // Calculate world position relative to grid center
    const vec2 worldPos = v_WorldPosition.xy - v_GridPosition.xy;
    
    // Offset to grid corner for proper grid alignment
    const vec2 gridPos = worldPos + v_GridSize * 0.5f;
    
    // Calculate coordinates in terms of 2x2 checker groups
    // Each "cell" in your old shader was 1 unit, but represented a 2x2 checker pattern
    const vec2 checkerGroupCoord = gridPos / (v_CellSize * 2.0f);
    
    // Get the position within each 2x2 group (0 to 1 range)
    const vec2 groupLocalPos = fract(checkerGroupCoord);
    
    // Calculate individual checker cell within the 2x2 group
    const ivec2 checkerCell = ivec2(floor(checkerGroupCoord * 2.0f));
    
    vec4 finalColor = vec4(0.0f);
    
    // Add checkerboard pattern if enabled
    if (v_ShowCheckerboard > 0.5f) {
        // Create 2x2 checkerboard pattern within each group
        const bool isEvenChecker = ((checkerCell.x + checkerCell.y) & 1) == 0;
        const vec4 checkerColor = isEvenChecker ? v_CheckerColor1 : v_CheckerColor2;
        finalColor = mix(finalColor, checkerColor, checkerColor.a);
    }
    
    // Create grid lines that surround each 2x2 group
    // Convert line width to group space
    const vec2 pixelSize = fwidth(checkerGroupCoord);
    const float lineWidth = v_LineWidth * max(pixelSize.x, pixelSize.y);
    
    // Check distance to group boundaries (lines surround each 2x2 group)
    const vec2 distToGroupEdge = min(groupLocalPos, 1.0f - groupLocalPos);
    const float minDistToEdge = min(distToGroupEdge.x, distToGroupEdge.y);
    
    // Create anti-aliased grid lines at group boundaries
    const float gridAlpha = 1.0f - smoothstep(0.0f, lineWidth, minDistToEdge);
    
    // Add grid lines on top
    finalColor = mix(finalColor, v_GridColor, gridAlpha * v_GridColor.a);
    
    // Discard fully transparent pixels for better performance
    if (finalColor.a < 0.01f) {
        discard;
    }
    
    FragColor = finalColor;
}