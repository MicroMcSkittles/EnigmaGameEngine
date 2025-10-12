#pragma once
#include "Enigma/Renderer/Shader.h"

namespace Enigma::Renderer {
	class DefaultShaders {
	public:
		static Shader* LoadMainShader();
		static Shader* LoadCircleShader();
		static Shader* LoadPostProcShader();
	};
}