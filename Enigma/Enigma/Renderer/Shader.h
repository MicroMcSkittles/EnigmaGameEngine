#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"

#include <string>
#include <vector>

namespace Enigma::Renderer {

	struct Uniform {
		std::string name;
		DataType type;
		void* data;
	};

	enum ShaderFlag {
		ShaderFlagNone = 0,
		ShaderFlagPathAsSource = BIT(0), // Treats paths as shader source
		ShaderFlagForceCompile = BIT(1), // Doesn't check for cache file and recompiles
	};

	struct ShaderStage {
		ShaderStageType type;
		std::string filename;
	};
	struct ShaderConfig{
		u32 flags = ShaderFlagNone;
		std::vector<ShaderStage> stages;
	};

	class Shader {
	public:
		static ref<Shader> Create(const ShaderConfig& config);

		virtual const std::vector<Uniform>& GetActiveUniforms() = 0;
		virtual Uniform& GetUniform(const std::string& name) = 0;
		virtual bool IsUniformActive(const std::string& name) = 0;

		virtual void SetUniform(const std::string& name, void* data) = 0;

		virtual void SetPushConstant(void* data, u32 offset, u32 size) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
	};

}