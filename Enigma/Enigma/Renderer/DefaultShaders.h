#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/Shader.h"

namespace Enigma::Renderer {
	class DefaultShaders {
	public:
		static ref<Shader> LoadMainShader();
		static ref<Shader> LoadCircleShader();
		static ref<Shader> LoadPostProcShader();
	};
}