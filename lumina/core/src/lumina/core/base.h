#pragma once

#include <memory>

namespace lumina
{
    template<typename T>
    using scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr scope<T> make_scope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr ref<T> make_ref(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
