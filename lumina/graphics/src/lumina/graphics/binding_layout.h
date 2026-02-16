#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <utility>
#include <vector>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    class texture;
    class uniform_buffer;

    /// Types of shader resource bindings.
    enum class binding_type
    {
        texture,            // Shader resource view (SRV) for textures
        sampler,            // Texture sampler
        constant_buffer,    // Uniform/constant buffer
        storage_texture,    // Read-write texture (UAV)
        storage_buffer      // Read-write buffer (UAV)
    };

    /// Describes a single binding slot in a binding layout.
    struct binding_item
    {
        uint32_t slot = 0;
        binding_type type = binding_type::texture;
        uint32_t array_size = 1;

        binding_item() = default;
        binding_item(uint32_t slot, binding_type type, uint32_t size = 1) : slot(slot), type(type), array_size(size) {}

        [[nodiscard]] static binding_item texture(uint32_t slot) noexcept { return { slot, binding_type::texture, 1 }; }
        [[nodiscard]] static binding_item texture_array(uint32_t slot, uint32_t count) noexcept { return { slot, binding_type::texture, count }; }
        [[nodiscard]] static binding_item sampler(uint32_t slot) noexcept { return { slot, binding_type::sampler, 1 }; }
        [[nodiscard]] static binding_item constant_buffer(uint32_t slot) noexcept { return { slot, binding_type::constant_buffer, 1 }; }
    };

    /// Configuration for binding layout creation.
    struct binding_layout_desc
    {
        std::vector<binding_item> bindings;
        bool vertex_shader_visible = true;
        bool pixel_shader_visible = true;

        binding_layout_desc& add_texture(uint32_t slot)
        {
            bindings.push_back(binding_item::texture(slot));
            return *this;
        }

        binding_layout_desc& add_texture_array(uint32_t slot, uint32_t count)
        {
            bindings.push_back(binding_item::texture_array(slot, count));
            return *this;
        }

        binding_layout_desc& add_sampler(uint32_t slot)
        {
            bindings.push_back(binding_item::sampler(slot));
            return *this;
        }

        binding_layout_desc& add_constant_buffer(uint32_t slot)
        {
            bindings.push_back(binding_item::constant_buffer(slot));
            return *this;
        }
    };

    /// GPU binding layout describing shader resource binding slots.
    /// Defines the structure of resource bindings without actual resources.
    class binding_layout
    {
    public:
        ~binding_layout();

        binding_layout(const binding_layout&) = delete;
        binding_layout& operator=(const binding_layout&) = delete;

        /// Creates a binding layout. Returns nullptr on failure.
        [[nodiscard]] static ref<binding_layout> create(core::device& dev, const binding_layout_desc& desc);

        [[nodiscard]] const binding_layout_desc& get_desc() const noexcept { return m_desc; }
        [[nodiscard]] nvrhi::IBindingLayout* get_layout() const noexcept { return m_handle.Get(); }

    private:
        binding_layout(core::device& dev, nvrhi::BindingLayoutHandle handle, const binding_layout_desc& desc)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::BindingLayoutHandle m_handle;
        binding_layout_desc m_desc;
    };

    /// Describes a resource bound to a specific slot.
    struct binding_set_item
    {
        uint32_t slot = 0;                          // Binding slot
        uint32_t array_index = 0;                   // Array index for array bindings
        binding_type type = binding_type::texture;  // Type of binding
        void* resource = nullptr;                   // Pointer to resource (texture, sampler, buffer, etc.)

        binding_set_item() = default;
        binding_set_item(uint32_t slot, binding_type type, void* res, uint32_t arr_idx = 0)
            : slot(slot), array_index(arr_idx), type(type), resource(res) {}
    };

    class texture;
    class sampler;

    /// Configuration for binding set creation.
    struct binding_set_desc
    {
        ref<binding_layout> layout;
        std::vector<binding_set_item> bindings;

        binding_set_desc& add_texture(uint32_t slot, ref<texture> tex);
        binding_set_desc& add_texture_array_element(uint32_t slot, uint32_t array_index, ref<texture> tex);
        binding_set_desc& add_sampler(uint32_t slot, ref<sampler> samp);
        binding_set_desc& add_constant_buffer(uint32_t slot, ref<uniform_buffer> ubo);
    };

    /// GPU binding set containing actual resource bindings.
    /// Associates concrete resources with binding layout slots.
    class binding_set
    {
    public:
        ~binding_set();

        binding_set(const binding_set&) = delete;
        binding_set& operator=(const binding_set&) = delete;

        /// Creates a binding set. Returns nullptr on failure.
        [[nodiscard]] static ref<binding_set> create(core::device& dev, const binding_set_desc& desc);

        [[nodiscard]] nvrhi::IBindingSet* get_binding_set() const noexcept { return m_handle.Get(); }

    private:
        binding_set(core::device& dev, nvrhi::BindingSetHandle handle, ref<binding_layout> layout)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_layout(layout)
        {}

        core::device& m_device;
        nvrhi::BindingSetHandle m_handle;
        ref<binding_layout> m_layout;
    };

    /// Common predefined binding layouts.
    namespace binding_layouts
    {
        /// Single texture + sampler (typical 2D sprite shader)
        [[nodiscard]] inline binding_layout_desc texture_sampler() noexcept
        {
            binding_layout_desc desc;
            desc.add_texture(0);
            desc.add_sampler(0);
            return desc;
        }

        /// Constant buffer only (transform matrices, etc.)
        [[nodiscard]] inline binding_layout_desc constant_buffer() noexcept
        {
            binding_layout_desc desc;
            desc.add_constant_buffer(0);
            return desc;
        }

        /// Constant buffer + texture + sampler
        [[nodiscard]] inline binding_layout_desc standard_2d() noexcept
        {
            binding_layout_desc desc;
            desc.add_constant_buffer(0);
            desc.add_texture(0);
            desc.add_sampler(0);
            return desc;
        }
    }
}
