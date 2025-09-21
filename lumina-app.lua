-- lumina.lua
workspace "Lumina"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "LuminaApplication"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Lumina External
include "lumina-external.lua"

-- App
group "App"
   include "applications/lumina-application"
group ""