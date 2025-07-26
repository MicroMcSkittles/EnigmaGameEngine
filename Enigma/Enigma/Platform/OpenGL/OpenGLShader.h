#pragma once
#include "Enigma/Renderer/Shader.h"
#include <map>

namespace Enigma {
	namespace Platform {
		namespace OpenGL {

			class OpenGLShader : public Renderer::Shader {
			public:
				OpenGLShader(const Renderer::ShaderConfig& config);
				~OpenGLShader();

				virtual const std::vector<Renderer::Uniform>& GetActiveUniforms() override;
				virtual Renderer::Uniform& GetUniform(const std::string& name) override;
				virtual bool IsUniformActive(const std::string& name) override;

				virtual void SetUniform(const std::string& name, void* data) override;

				virtual void Bind() override;
				virtual void Unbind() override;

			private:
				// Find all uniform locations, types, and names
				void FindUniforms();

			private:
				uint32_t m_Handle;
				uint32_t m_VertexHandle;
				uint32_t m_PixelHandle;
				uint32_t m_GeometryHandle;

				Renderer::ShaderConfig m_Config;

				std::vector<Renderer::Uniform> m_Uniforms;
				std::vector<uint32_t> m_UniformLocations;
				std::map<std::string, uint32_t> m_UniformLookup;
			};

		}
	}
}