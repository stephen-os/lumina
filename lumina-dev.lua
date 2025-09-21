-- lumina.lua
workspace "Lumina"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "LuminaApplication"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Lumina External
include "lumina-external.lua"

-- Projects
group "Demos"
   include "demos/simple-app"
   include "demos/basic-rendering"
   include "demos/renderer-2d"
   include "demos/renderer-3d"
   include "demos/physics"
   include "demos/point-lights"
group ""

-- App
group "Application"
   include "applications/lumina-application"
group ""