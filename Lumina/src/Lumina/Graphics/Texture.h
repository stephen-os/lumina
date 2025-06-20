#pragma once

#include <string>

#include "../Core/Ref.h"

namespace Lumina
{
	class Texture : public Referencable
	{
	public:
		static Ref<Texture> Create(std::string& source);
		static Ref<Texture> Create(uint32_t width, uint32_t height);

		Texture(std::string& source);
		Texture(uint32_t width, uint32_t height);
		~Texture();

		void Bind(uint32_t slot = 0) const ;
		void Unbind() const ;

		bool SetResolution(int width, int height) ;

		void SetData(void* data, uint32_t size);

		uint32_t GetID() const { return m_BufferID; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		std::string GetPath() const { return m_Path; }

	private:
		std::string m_Path;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_BufferID = 0;
	};
}