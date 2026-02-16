#pragma once

#include "event.h"

#include <string>

namespace lumina::core
{
    class layer
    {
    public:
        layer(const std::string& name = "unnamed_layer") : m_name(name) {}
        virtual ~layer() = default;

        virtual void on_attach() {}
        virtual void on_detach() {}
        virtual void on_update(float dt) {}
        virtual void on_render() {}
        virtual void on_event(event& e) {}

        [[nodiscard]] const std::string& get_name() const { return m_name; }

    private:
        std::string m_name;
    };
}
