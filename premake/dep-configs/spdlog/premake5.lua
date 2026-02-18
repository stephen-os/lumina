-- spdlog dependency configuration
-- spdlog is header-only, but we create a project for include path management

project "spdlog"
    kind "None"  -- Header-only library
    language "C++"

    -- No files to compile, just for include path management
    -- Projects that use spdlog should add:
    -- includedirs { "%{wks.location}/dependencies/spdlog/include" }
