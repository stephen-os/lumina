-- LuminaExternal.lua

IncludeDir = {}
IncludeDir["glm"] = "../Dependencies/glm"
IncludeDir["spdlog"] = "../Dependencies/spdlog/include"

Library = {}
Library["OpenGL"] = "opengl32.lib"

group "Dependencies"
   include "dependencies/imgui"
   include "dependencies/glfw"
   include "dependencies/glad"
   include "dependencies/tinygltf"
   include "dependencies/imguifd"
   include "dependencies/box2d"
group ""

group "Core"
   include "lumina"
group ""