#pragma once
#include "Enigma/Renderer/Shader.h"
#include <map>

#include <shaderc/shaderc.hpp>

namespace Enigma::Platform::OpenGL {

	struct OpenGLShaderStage {
		Renderer::ShaderStageType type;
		std::string filename;
		u64 checksum;

		u32 handle;
		std::vector<u32> vulkanSpirv;
		std::vector<u32> openglSpirv;
		std::string glslSource;
	};

	class OpenGLShader : public Renderer::Shader {
	public:
		OpenGLShader(const Renderer::ShaderConfig& config);
		~OpenGLShader();

		virtual const std::vector<Renderer::Uniform>& GetActiveUniforms() override;
		virtual Renderer::Uniform& GetUniform(const std::string& name) override;
		virtual bool IsUniformActive(const std::string& name) override;

		virtual void SetUniform(const std::string& name, void* data) override;

		virtual void SetPushConstant(void* data, u32 offset, u32 size) override;

		virtual void Bind() override;
		virtual void Unbind() override;

	private:

		bool IsStageVKSpirvCached(const OpenGLShaderStage& stage);
		bool LoadStageVKSpirvCache(OpenGLShaderStage& stage);
		bool CompileStageToVKSpirv(OpenGLShaderStage& stage);

		bool IsStageGLSpirvCached(const OpenGLShaderStage& stage);
		bool LoadStageGLSpirvCache(OpenGLShaderStage& stage);
		bool CompileStageToGLSpirv(OpenGLShaderStage& stage);

		void CreateProgram();

		void ReflectStage(const OpenGLShaderStage& stage);

		void VarifyCacheDirectory();

	private:
		u32 m_Handle;

		Renderer::ShaderConfig m_Config;
		std::vector<OpenGLShaderStage> m_Stages;
		u32 m_PushConstantCount;

		std::vector<Renderer::Uniform> m_Uniforms;
		std::vector<u32> m_UniformLocations;
		std::map<std::string, u32> m_UniformLookup;

		const u64 c_CacheChecksumSize = sizeof(u64);
		const std::string c_VKSpirvCacheExt = ".vk.spirv";
		const std::string c_GLSpirvCacheExt = ".gl.spirv";
		const std::string c_CacheDirectory = "./cache/shader/";
	};
}