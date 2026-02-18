-- NVRHI dependency configuration (D3D12 only)
-- Builds NVRHI core and D3D12 backend as static libraries

local nvrhi_root = "%{wks.location}/dependencies/nvrhi"

-- Common NVRHI library (core + validation)
project "nvrhi"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        nvrhi_root .. "/src/common/*.cpp",
        nvrhi_root .. "/src/validation/*.cpp",
        nvrhi_root .. "/include/nvrhi/*.h",
        nvrhi_root .. "/include/nvrhi/common/*.h",
    }

    includedirs {
        nvrhi_root .. "/include",
        nvrhi_root .. "/thirdparty/DirectX-Headers/include",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "speed"

    filter "configurations:Dist"
        runtime "Release"
        optimize "speed"
        symbols "off"

-- D3D12 Backend
project "nvrhi-d3d12"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        nvrhi_root .. "/src/d3d12/*.cpp",
        nvrhi_root .. "/include/nvrhi/d3d12.h",
    }

    includedirs {
        nvrhi_root .. "/include",
        nvrhi_root .. "/thirdparty/DirectX-Headers/include",
        nvrhi_root .. "/thirdparty/DirectX-Headers/include/directx",
    }

    defines {
        "NVRHI_WITH_DX12=1"
    }

    links {
        "nvrhi",
        "d3d12",
        "dxgi",
        "dxguid",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "speed"

    filter "configurations:Dist"
        runtime "Release"
        optimize "speed"
        symbols "off"
