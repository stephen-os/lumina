-- premake5.lua
workspace "Lumina"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Example"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "LuminaExternal.lua"

-- Projects

group "Projects"
   include "Projects/Example"
   include "Projects/BasicRendering"
   include "Projects/Renderer2D"
group ""