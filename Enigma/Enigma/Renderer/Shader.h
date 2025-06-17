#pragma once
#include "Renderer/RenderEnum.h"
#include "Core/IdHandler.h"
#include <stdint.h>
#include <string>
#include <vector>

namespace Enigma {
	namespace Renderer {

		enum ShaderFlag {
			None = 0,
			VertexShader = BIT(0),
			PixelShader = BIT(1),
			GeometryShader = BIT(2),
			PathAsSource = BIT(3) // Treats paths as shader source
		};

		struct Uniform {
			std::string name;
			DataType type;
			void* data;
		};

		struct ShaderConfig{
			uint32_t flags = VertexShader | PixelShader;
			std::string vertexPath;
			std::string pixelPath;
			std::string geomatryPath;
		};

		class Shader {
		public:
			static Shader* Create(const ShaderConfig& config);
			Core::ID GetID() { return m_ID; }

			virtual const std::vector<Uniform>& GetActiveUniforms() = 0;
			virtual Uniform& GetUniform(const std::string& name) = 0;
			virtual bool IsUniformActive(const std::string& name) = 0;

			virtual void SetUniform(const std::string& name, void* data) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

		private:
			Core::ID m_ID;

		private:
			inline static Core::IDHandler<Shader*> s_IDHandler;
		};

	}
}