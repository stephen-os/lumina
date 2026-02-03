// Grid shader for 2D rendering
// Procedural grid rendering in fragment shader
// Supports optional checkerboard pattern
// Positions are pre-transformed on CPU

struct VSInput
{
    float4 Position : POSITION;         // Pre-transformed clip-space position
    float4 LocalPosition : LOCALPOS;    // Local UV for grid calculation
    float4 LineColor : LINECOLOR;
    float2 GridSize : GRIDSIZE;
    float CellSize : CELLSIZE;
    float LineWidth : LINEWIDTH;
    float ShowCheckerboard : CHECKER;
    float4 CheckerColor1 : CHECKERCOLOR1;
    float4 CheckerColor2 : CHECKERCOLOR2;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 LocalPosition : LOCALPOS;
    float4 LineColor : LINECOLOR;
    float2 GridSize : GRIDSIZE;
    float CellSize : CELLSIZE;
    float LineWidth : LINEWIDTH;
    float ShowCheckerboard : CHECKER;
    float4 CheckerColor1 : CHECKERCOLOR1;
    float4 CheckerColor2 : CHECKERCOLOR2;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = input.Position;
    output.LocalPosition = input.LocalPosition;
    output.LineColor = input.LineColor;
    output.GridSize = input.GridSize;
    output.CellSize = input.CellSize;
    output.LineWidth = input.LineWidth;
    output.ShowCheckerboard = input.ShowCheckerboard;
    output.CheckerColor1 = input.CheckerColor1;
    output.CheckerColor2 = input.CheckerColor2;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 localPos = input.LocalPosition.xy;
    float2 gridSize = input.GridSize;
    float cellSize = input.CellSize;
    float lineWidth = input.LineWidth;

    // Scale local position to grid coordinates
    float2 gridCoord = localPos * gridSize / cellSize;

    // Calculate grid lines using frac
    float2 gridLines = abs(frac(gridCoord - 0.5) - 0.5) / fwidth(gridCoord);
    float minLine = min(gridLines.x, gridLines.y);

    // Line intensity
    float lineAlpha = 1.0 - min(minLine / lineWidth, 1.0);

    // Base color (checkerboard or transparent)
    float4 baseColor = float4(0, 0, 0, 0);

    if (input.ShowCheckerboard > 0.5)
    {
        // Calculate checkerboard pattern
        float2 cellIndex = floor(gridCoord);
        float checker = fmod(cellIndex.x + cellIndex.y, 2.0);
        baseColor = lerp(input.CheckerColor1, input.CheckerColor2, checker);
    }

    // Blend grid lines over base
    float4 finalColor = lerp(baseColor, input.LineColor, lineAlpha);

    // Discard fully transparent pixels
    if (finalColor.a < 0.001)
        discard;

    return finalColor;
}
