-- LuminaExternal.lua

IncludeDir = {}
IncludeDir["glm"] = "../Dependencies/glm"
IncludeDir["spdlog"] = "../Dependencies/spdlog/include"

Library = {}

group "Dependencies"
   include "dependencies/imgui"
   include "dependencies/imguifd"
   include "dependencies/imgui-node-editor"
   include "dependencies/glfw"
   include "dependencies/tinygltf"
   include "dependencies/box2d"
   include "dependencies/libuiohook"
   include "dependencies/nvrhi"
group ""

group "Core"
   include "lumina"
group ""