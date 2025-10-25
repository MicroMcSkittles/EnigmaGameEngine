#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"

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
			u32 flags = VertexShader | PixelShader;
			std::string vertexPath;
			std::string pixelPath;
			std::string geomatryPath;
		};

		class Shader {
		public:
			static ref<Shader> Create(const ShaderConfig& config);

			virtual const std::vector<Uniform>& GetActiveUniforms() = 0;
			virtual Uniform& GetUniform(const std::string& name) = 0;
			virtual bool IsUniformActive(const std::string& name) = 0;

			virtual void SetUniform(const std::string& name, void* data) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

	}
}