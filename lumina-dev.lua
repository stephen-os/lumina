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
   include "demos/basic-rendering"
   include "demos/advanced-rendering"
   include "demos/spacial-rendering"
   include "demos/physics"
   include "demos/point-lights"
group ""

-- App
group "Application"
   include "applications/lumina-application"
group ""