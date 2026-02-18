#pragma once

#include <cstdint>
#include <chrono>
#include <initializer_list>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lumina::core
{
    class random
    {
    public:
        random() = default;
        explicit random(uint32_t seed) noexcept : m_state(seed) {}

        void set_seed(uint32_t seed) noexcept { m_state = seed; }
        uint32_t get_seed() const noexcept { return m_state; }

        void seed_from_time() noexcept
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
            m_state = static_cast<uint32_t>(timestamp);
        }

        uint32_t next_uint32() noexcept
        {
            m_state = (lcg_a * m_state + lcg_c) % lcg_m;
            return m_state;
        }

        // --- Integer ---

        int32_t int32() noexcept { return static_cast<int32_t>(next_uint32()); }
        uint32_t uint32() noexcept { return next_uint32(); }
        int32_t int32(int32_t max) noexcept { return int32(0, max); }

        int32_t int32(int32_t min, int32_t max) noexcept
        {
            if (min >= max) return min;
            return min + static_cast<int32_t>(next_uint32() % static_cast<::uint32_t>(max - min + 1));
        }

        // --- Float ---

        float floating() noexcept
        {
            return static_cast<float>(next_uint32()) / static_cast<float>(lcg_m);
        }

        float floating(float max) noexcept
        {
            return floating() * max;
        }

        float floating(float min, float max) noexcept
        {
            return min + floating() * (max - min);
        }

        // --- Double ---

        double real() noexcept
        {
            return static_cast<double>(next_uint32()) / static_cast<double>(lcg_m);
        }

        double real(double max) noexcept
        {
            return real() * max;
        }

        double real(double min, double max) noexcept
        {
            return min + real() * (max - min);
        }

        // --- Bool ---

        bool boolean() noexcept
        {
            return (next_uint32() & 1) == 1;
        }

        bool boolean(float probability) noexcept
        {
            return floating() < probability;
        }

        // --- GLM vectors ---

        glm::vec2 vec2() noexcept { return { floating(), floating() }; }
        glm::vec2 vec2(float max) noexcept { return { floating(max), floating(max) }; }
        glm::vec2 vec2(float min, float max) noexcept { return { floating(min, max), floating(min, max) }; }
        glm::vec2 vec2(const glm::vec2& min, const glm::vec2& max) noexcept
        {
            return { floating(min.x, max.x), floating(min.y, max.y) };
        }

        glm::vec3 vec3() noexcept { return { floating(), floating(), floating() }; }
        glm::vec3 vec3(float max) noexcept { return { floating(max), floating(max), floating(max) }; }
        glm::vec3 vec3(float min, float max) noexcept { return { floating(min, max), floating(min, max), floating(min, max) }; }
        glm::vec3 vec3(const glm::vec3& min, const glm::vec3& max) noexcept
        {
            return { floating(min.x, max.x), floating(min.y, max.y), floating(min.z, max.z) };
        }

        glm::vec4 vec4() noexcept { return { floating(), floating(), floating(), floating() }; }
        glm::vec4 vec4(float max) noexcept { return { floating(max), floating(max), floating(max), floating(max) }; }
        glm::vec4 vec4(float min, float max) noexcept { return { floating(min, max), floating(min, max), floating(min, max), floating(min, max) }; }
        glm::vec4 vec4(const glm::vec4& min, const glm::vec4& max) noexcept
        {
            return { floating(min.x, max.x), floating(min.y, max.y), floating(min.z, max.z), floating(min.w, max.w) };
        }

        // --- Geometric ---

        glm::vec2 unit_circle() noexcept
        {
            float angle = floating(0.0f, glm::two_pi<float>());
            return { glm::cos(angle), glm::sin(angle) };
        }

        glm::vec2 inside_circle() noexcept
        {
            float radius = glm::sqrt(floating());
            float angle = floating(0.0f, glm::two_pi<float>());
            return radius * glm::vec2(glm::cos(angle), glm::sin(angle));
        }

        glm::vec2 inside_circle(float radius) noexcept
        {
            return inside_circle() * radius;
        }

        // --- Color ---

        glm::vec3 color() noexcept { return vec3(); }
        glm::vec4 color_with_alpha() noexcept { return vec4(); }

        glm::vec3 color_variation(const glm::vec3& base_color, float variation = 0.1f) noexcept
        {
            glm::vec3 offset = vec3(-variation, variation);
            return glm::clamp(base_color + offset, 0.0f, 1.0f);
        }

        // --- Angle ---

        float angle() noexcept { return floating(0.0f, glm::two_pi<float>()); }
        float angle_degrees() noexcept { return floating(0.0f, 360.0f); }

        // --- Container ---

        template<typename Container>
        auto& choose(Container& container) noexcept
        {
            auto size = static_cast<int32_t>(container.size());
            auto index = int32(0, size - 1);
            return container[index];
        }

        template<typename Container>
        const auto& choose(const Container& container) noexcept
        {
            auto size = static_cast<int32_t>(container.size());
            auto index = int32(0, size - 1);
            return container[index];
        }

        template<typename T>
        T choose(std::initializer_list<T> choices) noexcept
        {
            auto size = static_cast<int32_t>(choices.size());
            auto index = int32(0, size - 1);
            return *(choices.begin() + index);
        }

    private:
        uint32_t m_state = default_seed;

        static constexpr uint32_t default_seed = 1;
        static constexpr uint32_t lcg_a = 48271;
        static constexpr uint32_t lcg_c = 0;
        static constexpr uint32_t lcg_m = 2147483647; // 2^31 - 1
    };
}
