-- lumina.lua
workspace "Lumina"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Example"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Lumina External
include "lumina-external.lua"

-- Projects
group "Projects"
   include "projects/example/example.lua"
   include "projects/basic-rendering/basic-rendering.lua"
   include "projects/renderer-2d/renderer-2d.lua"
   include "projects/renderer-3d/renderer-3d.lua"
   include "projects/box2d-demo/box2d-demo.lua"
group ""