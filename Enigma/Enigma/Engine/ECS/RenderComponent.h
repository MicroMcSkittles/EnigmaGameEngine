#pragma once
#include "Enigma/Engine/ECS/Component.h"
#include "Enigma/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Engine {
		namespace ECS {

			struct Render2D {
				COMP_DEF(Render2D);

				int depth = 0;
				glm::vec4 tint = { 1,1,1,1 };
				Renderer::Texture* texture = nullptr;
			};

			// Not impl yet
			struct Camera {
				COMP_DEF(Camera);

				bool active = true;

				float near = 0.1f;
				float far = 1000.0f;
			};

		}
	}
}