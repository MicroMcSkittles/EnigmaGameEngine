#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/EntityComponentSystem.h"
#include "Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Engine {
		namespace ECS {

			class Render2D : public Component {
			public:
				COMP_DEF(Render2D);

				Render2D(
					int depth = 0,
					Renderer::Texture* texture = nullptr,
					const glm::vec4& tint = { 1, 1, 1, 1 }
				) : m_Depth(depth), m_Texture(texture), m_Tint(tint)
				{ }

				int& GetDepth() { return m_Depth; }
				Renderer::Texture*& GetTexture() { return m_Texture; }
				glm::vec4& GetTint() { return m_Tint; }

			private:
				int m_Depth;
				Renderer::Texture* m_Texture;
				glm::vec4 m_Tint;
			};

		}
	}
}