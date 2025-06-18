#include "Texture.h"
#include "OpenGL/OpenGLTexture.h"
#include "../Core/API.h"

namespace Lumina
{
    Ref<Texture> Texture::Create(std::string& source)
    {
        switch (RendererAPI::GetAPI())
        {
        case API::OPENGL: return Ref<OpenGLTexture>::Create(source);
        default: return nullptr;
        }
    }

    Ref<Texture> Texture::Create(uint32_t width, uint32_t height)
    {
        switch (RendererAPI::GetAPI())
        {
        case API::OPENGL: return Ref<OpenGLTexture>::Create(width, height);
        default: return nullptr;
        }
    }
}
