#pragma once

#include <cstdint>

namespace Lumina
{
    namespace TextureSlots
    {
        // Material texture slots (0-7)
        constexpr uint32_t MATERIAL_ALBEDO = 0;
        constexpr uint32_t MATERIAL_NORMAL = 1;
        constexpr uint32_t MATERIAL_METALLIC = 2;
        constexpr uint32_t MATERIAL_ROUGHNESS = 3;
        constexpr uint32_t MATERIAL_AO = 4;
        constexpr uint32_t MATERIAL_EMISSION = 5;
        constexpr uint32_t MATERIAL_HEIGHT = 6;
        constexpr uint32_t MATERIAL_ALPHA = 7;

        // Environment texture slots (8-15)
        constexpr uint32_t SKYBOX = 8;
        constexpr uint32_t ENVIRONMENT_MAP = 9;
        constexpr uint32_t IRRADIANCE_MAP = 10;
        constexpr uint32_t PREFILTERED_ENV = 11;
        constexpr uint32_t BRDF_LUT = 12;
        constexpr uint32_t SHADOW_MAP_0 = 13;
        constexpr uint32_t SHADOW_MAP_1 = 14;
        constexpr uint32_t SHADOW_MAP_2 = 15;

        // Effect texture slots (16-23)
        constexpr uint32_t POST_PROCESS_0 = 16;
        constexpr uint32_t POST_PROCESS_1 = 17;
        constexpr uint32_t POST_PROCESS_2 = 18;
        constexpr uint32_t POST_PROCESS_3 = 19;
        constexpr uint32_t BLOOM_TEXTURE = 20;
        constexpr uint32_t DEPTH_BUFFER = 21;
        constexpr uint32_t NORMAL_BUFFER = 22;
        constexpr uint32_t VELOCITY_BUFFER = 23;

        // Compute/Temporary slots (24-31)
        constexpr uint32_t COMPUTE_0 = 24;
        constexpr uint32_t COMPUTE_1 = 25;
        constexpr uint32_t COMPUTE_2 = 26;
        constexpr uint32_t COMPUTE_3 = 27;
        constexpr uint32_t TEMPORARY_0 = 28;
        constexpr uint32_t TEMPORARY_1 = 29;
        constexpr uint32_t TEMPORARY_2 = 30;
        constexpr uint32_t TEMPORARY_3 = 31;
    }
}