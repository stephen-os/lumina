#pragma once

#include <string>
#include <memory>

#include "../Core/Ref.h"

namespace Lumina
{
    class Texture : public Referencable
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual bool SetResolution(int width, int height) = 0;

        virtual void SetData(void* data, uint32_t size) = 0;

        virtual uint32_t GetID() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual std::string GetPath() const = 0; 

        static Ref<Texture> Create(std::string& source);
        static Ref<Texture> Create(uint32_t width, uint32_t height);
    };
}
