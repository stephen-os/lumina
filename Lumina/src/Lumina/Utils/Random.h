#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace Lumina
{
    class Random
    {
    public:
        Random() = default;
        explicit Random(uint32_t seed) noexcept : m_State(seed) {}

        inline void SetSeed(uint32_t seed) noexcept { m_State = seed; }
        inline uint32_t GetSeed() const noexcept { return m_State; }
        inline void SeedFromTime() noexcept
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
            m_State = static_cast<uint32_t>(timestamp);
        }

        inline uint32_t NextUInt32() noexcept
        {
            m_State = (LCG_A * m_State + LCG_C) % LCG_M;
            return m_State;
        }

        inline int32_t Int32() noexcept { return static_cast<int32_t>(NextUInt32()); }
        inline uint32_t UInt32() noexcept { return NextUInt32(); }
        inline int32_t Int32(int32_t max) noexcept { return Int32(0, max); }
        inline int32_t Int32(int32_t min, int32_t max) noexcept
        {
            if (min >= max) return min;
            return min + static_cast<int32_t>(NextUInt32() % static_cast<uint32_t>(max - min + 1));
        }

        inline float Float() noexcept
        {
            return static_cast<float>(NextUInt32()) / static_cast<float>(LCG_M);
        }

        inline float Float(float max) noexcept
        {
            return Float() * max;
        }

        inline float Float(float min, float max) noexcept
        {
            return min + Float() * (max - min);
        }

        inline double Double() noexcept
        {
            return static_cast<double>(NextUInt32()) / static_cast<double>(LCG_M);
        }

        inline double Double(double max) noexcept
        {
            return Double() * max;
        }

        inline double Double(double min, double max) noexcept
        {
            return min + Double() * (max - min);
        }

        inline bool Bool() noexcept
        {
            return (NextUInt32() & 1) == 1;
        }

        inline bool Bool(float probability) noexcept
        {
            return Float() < probability;
        }

        inline glm::vec2 Vec2() noexcept
        {
            return { Float(), Float() };
        }

        inline glm::vec2 Vec2(float max) noexcept
        {
            return { Float(max), Float(max) };
        }

        inline glm::vec2 Vec2(float min, float max) noexcept
        {
            return { Float(min, max), Float(min, max) };
        }

        inline glm::vec2 Vec2(const glm::vec2& min, const glm::vec2& max) noexcept
        {
            return { Float(min.x, max.x), Float(min.y, max.y) };
        }

        inline glm::vec3 Vec3() noexcept
        {
            return { Float(), Float(), Float() };
        }

        inline glm::vec3 Vec3(float max) noexcept
        {
            return { Float(max), Float(max), Float(max) };
        }

        inline glm::vec3 Vec3(float min, float max) noexcept
        {
            return { Float(min, max), Float(min, max), Float(min, max) };
        }

        inline glm::vec3 Vec3(const glm::vec3& min, const glm::vec3& max) noexcept
        {
            return { Float(min.x, max.x), Float(min.y, max.y), Float(min.z, max.z) };
        }

        inline glm::vec4 Vec4() noexcept
        {
            return { Float(), Float(), Float(), Float() };
        }

        inline glm::vec4 Vec4(float max) noexcept
        {
            return { Float(max), Float(max), Float(max), Float(max) };
        }

        inline glm::vec4 Vec4(float min, float max) noexcept
        {
            return { Float(min, max), Float(min, max), Float(min, max), Float(min, max) };
        }

        inline glm::vec4 Vec4(const glm::vec4& min, const glm::vec4& max) noexcept
        {
            return { Float(min.x, max.x), Float(min.y, max.y), Float(min.z, max.z), Float(min.w, max.w) };
        }

        inline glm::vec2 UnitCircle() noexcept
        {
            float angle = Float(0.0f, glm::two_pi<float>());
            return { glm::cos(angle), glm::sin(angle) };
        }

        inline glm::vec2 InsideCircle() noexcept
        {
            float radius = glm::sqrt(Float()); // sqrt for uniform distribution
            float angle = Float(0.0f, glm::two_pi<float>());
            return radius * glm::vec2(glm::cos(angle), glm::sin(angle));
        }

        inline glm::vec2 InsideCircle(float radius) noexcept
        {
            return InsideCircle() * radius;
        }

        inline glm::vec3 Color() noexcept
        {
            return Vec3();
        }

        inline glm::vec4 ColorWithAlpha() noexcept
        {
            return Vec4();
        }

        inline glm::vec3 ColorVariation(const glm::vec3& baseColor, float variation = 0.1f) noexcept
        {
            glm::vec3 offset = Vec3(-variation, variation);
            return glm::clamp(baseColor + offset, 0.0f, 1.0f);
        }

        inline float Angle() noexcept
        {
            return Float(0.0f, glm::two_pi<float>());
        }

        inline float AngleDegrees() noexcept
        {
            return Float(0.0f, 360.0f);
        }

        template<typename Container>
        inline auto& Choose(Container& container) noexcept
        {
            auto size = static_cast<int32_t>(container.size());
            auto index = Int32(0, size - 1);
            return container[index];
        }

        template<typename Container>
        inline const auto& Choose(const Container& container) noexcept
        {
            auto size = static_cast<int32_t>(container.size());
            auto index = Int32(0, size - 1);
            return container[index];
        }

        template<typename T>
        inline T Choose(std::initializer_list<T> choices) noexcept
        {
            auto size = static_cast<int32_t>(choices.size());
            auto index = Int32(0, size - 1);
            return *(choices.begin() + index);
        }

    private:
        uint32_t m_State = DEFAULT_SEED;

        static constexpr uint32_t DEFAULT_SEED = 1;
        static constexpr uint32_t LCG_A =48271;
        static constexpr uint32_t LCG_C = 0;
        static constexpr uint32_t LCG_M = 2147483647; // 2^31 - 1
    };
}