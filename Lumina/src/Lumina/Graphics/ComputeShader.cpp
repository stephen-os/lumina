#include "ComputeShader.h"

#include "OpenGL/OpenGLComputeShader.h"

#include "../Core/API.h"

namespace Lumina
{
	std::shared_ptr<ComputeShader> ComputeShader::Create(const std::string& source)
	{
		switch (RendererAPI::GetAPI())
		{
		case API::OPENGL: return std::make_shared<OpenGLComputeShader>(source);
		default: return nullptr;
		}
	}
}
