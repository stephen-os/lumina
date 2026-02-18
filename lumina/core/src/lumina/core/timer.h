#pragma once

#include "log.h"

#include <chrono>
#include <string>

namespace lumina::core
{
    class timer
    {
    public:
        timer() { reset(); }

        void reset()
        {
            m_start = std::chrono::high_resolution_clock::now();
        }

        [[nodiscard]] float elapsed() const
        {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<float>(now - m_start).count();
        }

        [[nodiscard]] float elapsed_millis() const
        {
            return elapsed() * 1000.0f;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };

    class scoped_timer
    {
    public:
        explicit scoped_timer(const std::string& name)
            : m_name(name) {}

        ~scoped_timer()
        {
            float ms = m_timer.elapsed_millis();
            LUMINA_LOG_TRACE("[TIMER] {} - {:.3f}ms", m_name, ms);
        }

        scoped_timer(const scoped_timer&) = delete;
        scoped_timer& operator=(const scoped_timer&) = delete;

    private:
        std::string m_name;
        timer m_timer;
    };
}
