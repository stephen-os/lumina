#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <vector>

namespace nvrhi
{
    class IBindingLayout;
    class IBindingSet;
}
namespace lumina::core { class device; }

namespace lumina::graphics
{
    class texture;
    class uniform_buffer;

    enum class binding_type
    {
        texture,            // Shader resource view (SRV) for textures
        sampler,            // Texture sampler
        constant_buffer,    // Uniform/constant buffer
        storage_texture,    // Read-write texture (UAV)
        storage_buffer      // Read-write buffer (UAV)
    };

    struct binding_item
    {
        uint32_t slot = 0;
        binding_type type = binding_type::texture;
        uint32_t array_size = 1;  

        binding_item() = default;
        binding_item(uint32_t slot, binding_type type, uint32_t size = 1) : slot(slot), type(type), array_size(size) {}

        static binding_item texture(uint32_t slot) { return { slot, binding_type::texture, 1 }; }
        static binding_item texture_array(uint32_t slot, uint32_t count) { return { slot, binding_type::texture, count }; }
        static binding_item sampler(uint32_t slot) { return { slot, binding_type::sampler, 1 }; }
        static binding_item constant_buffer(uint32_t slot) { return { slot, binding_type::constant_buffer, 1 }; }
    };

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

    class binding_layout
    {
    public:
        ~binding_layout();

        binding_layout(const binding_layout&) = delete;
        binding_layout& operator=(const binding_layout&) = delete;

        static ref<binding_layout> create(core::device& dev, const binding_layout_desc& desc);

        const binding_layout_desc& get_desc() const { return m_desc; }

        nvrhi::IBindingLayout* get_layout() const { return m_handle; }

    private:
        binding_layout(core::device& dev, nvrhi::IBindingLayout* handle, const binding_layout_desc& desc)
            : m_device(dev)
            , m_handle(handle)
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::IBindingLayout* m_handle;
        binding_layout_desc m_desc;
    };

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

    struct binding_set_desc
    {
        ref<binding_layout> layout;
        std::vector<binding_set_item> bindings;

        binding_set_desc& add_texture(uint32_t slot, ref<texture> tex);
        binding_set_desc& add_texture_array_element(uint32_t slot, uint32_t array_index, ref<texture> tex);
        binding_set_desc& add_sampler(uint32_t slot, ref<sampler> samp);
        binding_set_desc& add_constant_buffer(uint32_t slot, ref<uniform_buffer> ubo);
    };

    class binding_set
    {
    public:
        ~binding_set();

        binding_set(const binding_set&) = delete;
        binding_set& operator=(const binding_set&) = delete;

        static ref<binding_set> create(core::device& dev, const binding_set_desc& desc);

        nvrhi::IBindingSet* get_binding_set() const { return m_handle; }

    private:
        binding_set(core::device& dev, nvrhi::IBindingSet* handle, ref<binding_layout> layout)
            : m_device(dev)
            , m_handle(handle)
            , m_layout(layout)
        {}

        core::device& m_device;
        nvrhi::IBindingSet* m_handle;
        ref<binding_layout> m_layout;
    };

    // Common predefined binding layouts
    namespace binding_layouts
    {
        // Single texture + sampler (typical 2D sprite shader)
        inline binding_layout_desc texture_sampler()
        {
            binding_layout_desc desc;
            desc.add_texture(0);
            desc.add_sampler(0);
            return desc;
        }

        // Constant buffer only (transform matrices, etc.)
        inline binding_layout_desc constant_buffer()
        {
            binding_layout_desc desc;
            desc.add_constant_buffer(0);
            return desc;
        }

        // Constant buffer + texture + sampler
        inline binding_layout_desc standard_2d()
        {
            binding_layout_desc desc;
            desc.add_constant_buffer(0);
            desc.add_texture(0);
            desc.add_sampler(0);
            return desc;
        }
    }
}
