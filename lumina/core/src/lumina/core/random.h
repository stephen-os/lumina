#pragma once

#include <cstdint>
#include <chrono>
#include <initializer_list>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Lumina
{
	class Random
	{
	public:
		Random() = default;
		explicit Random(uint32_t seed) noexcept : m_State(seed) {}

		void SetSeed(uint32_t seed) noexcept { m_State = seed; }
		uint32_t GetSeed() const noexcept { return m_State; }

		void SeedFromTime() noexcept
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
			m_State = static_cast<uint32_t>(timestamp);
		}

		uint32_t NextUint32() noexcept
		{
			m_State = (LcgA * m_State + LcgC) % LcgM;
			return m_State;
		}

		// --- Integer ---

		int32_t Int32() noexcept { return static_cast<int32_t>(NextUint32()); }
		uint32_t Uint32() noexcept { return NextUint32(); }
		int32_t Int32(int32_t max) noexcept { return Int32(0, max); }

		int32_t Int32(int32_t min, int32_t max) noexcept
		{
			if (min >= max) return min;
			return min + static_cast<int32_t>(NextUint32() % static_cast<::uint32_t>(max - min + 1));
		}

		// --- Float ---

		float Float() noexcept
		{
			return static_cast<float>(NextUint32()) / static_cast<float>(LcgM);
		}

		float Float(float max) noexcept
		{
			return Float() * max;
		}

		float Float(float min, float max) noexcept
		{
			return min + Float() * (max - min);
		}

		// --- Double ---

		double Real() noexcept
		{
			return static_cast<double>(NextUint32()) / static_cast<double>(LcgM);
		}

		double Real(double max) noexcept
		{
			return Real() * max;
		}

		double Real(double min, double max) noexcept
		{
			return min + Real() * (max - min);
		}

		// --- Bool ---

		bool Boolean() noexcept
		{
			return (NextUint32() & 1) == 1;
		}

		bool Boolean(float probability) noexcept
		{
			return Float() < probability;
		}

		// --- GLM vectors ---

		glm::vec2 Vec2() noexcept { return { Float(), Float() }; }
		glm::vec2 Vec2(float max) noexcept { return { Float(max), Float(max) }; }
		glm::vec2 Vec2(float min, float max) noexcept { return { Float(min, max), Float(min, max) }; }
		glm::vec2 Vec2(const glm::vec2& min, const glm::vec2& max) noexcept
		{
			return { Float(min.x, max.x), Float(min.y, max.y) };
		}

		glm::vec3 Vec3() noexcept { return { Float(), Float(), Float() }; }
		glm::vec3 Vec3(float max) noexcept { return { Float(max), Float(max), Float(max) }; }
		glm::vec3 Vec3(float min, float max) noexcept { return { Float(min, max), Float(min, max), Float(min, max) }; }
		glm::vec3 Vec3(const glm::vec3& min, const glm::vec3& max) noexcept
		{
			return { Float(min.x, max.x), Float(min.y, max.y), Float(min.z, max.z) };
		}

		glm::vec4 Vec4() noexcept { return { Float(), Float(), Float(), Float() }; }
		glm::vec4 Vec4(float max) noexcept { return { Float(max), Float(max), Float(max), Float(max) }; }
		glm::vec4 Vec4(float min, float max) noexcept { return { Float(min, max), Float(min, max), Float(min, max), Float(min, max) }; }
		glm::vec4 Vec4(const glm::vec4& min, const glm::vec4& max) noexcept
		{
			return { Float(min.x, max.x), Float(min.y, max.y), Float(min.z, max.z), Float(min.w, max.w) };
		}

		// --- Geometric ---

		glm::vec2 UnitCircle() noexcept
		{
			float angle = Float(0.0f, glm::two_pi<float>());
			return { glm::cos(angle), glm::sin(angle) };
		}

		glm::vec2 InsideCircle() noexcept
		{
			float radius = glm::sqrt(Float());
			float angle = Float(0.0f, glm::two_pi<float>());
			return radius * glm::vec2(glm::cos(angle), glm::sin(angle));
		}

		glm::vec2 InsideCircle(float radius) noexcept
		{
			return InsideCircle() * radius;
		}

		// --- Color ---

		glm::vec3 Color() noexcept { return Vec3(); }
		glm::vec4 ColorWithAlpha() noexcept { return Vec4(); }

		glm::vec3 ColorVariation(const glm::vec3& baseColor, float variation = 0.1f) noexcept
		{
			glm::vec3 offset = Vec3(-variation, variation);
			return glm::clamp(baseColor + offset, 0.0f, 1.0f);
		}

		// --- Angle ---

		float Angle() noexcept { return Float(0.0f, glm::two_pi<float>()); }
		float AngleDegrees() noexcept { return Float(0.0f, 360.0f); }

		// --- Container ---

		template<typename Container>
		auto& Choose(Container& container) noexcept
		{
			auto size = static_cast<int32_t>(container.size());
			auto index = Int32(0, size - 1);
			return container[index];
		}

		template<typename Container>
		const auto& Choose(const Container& container) noexcept
		{
			auto size = static_cast<int32_t>(container.size());
			auto index = Int32(0, size - 1);
			return container[index];
		}

		template<typename T>
		T Choose(std::initializer_list<T> choices) noexcept
		{
			auto size = static_cast<int32_t>(choices.size());
			auto index = Int32(0, size - 1);
			return *(choices.begin() + index);
		}

	private:
		uint32_t m_State = DefaultSeed;

		static constexpr uint32_t DefaultSeed = 1;
		static constexpr uint32_t LcgA = 48271;
		static constexpr uint32_t LcgC = 0;
		static constexpr uint32_t LcgM = 2147483647; // 2^31 - 1
	};
}
