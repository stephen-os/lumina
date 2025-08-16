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
   include "projects/demo-simple-app"
   include "projects/demo-basic-rendering"
   include "projects/demo-renderer-2d"
   include "projects/demo-renderer-3d"
   include "projects/demo-box-2d/"
   include "projects/demo-point-lights"
group ""