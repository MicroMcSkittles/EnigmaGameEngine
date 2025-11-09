#pragma once
#include "Enigma/Engine/ECS/EntityComponentSystem.h"
#include "Enigma/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Enigma::Engine::ECS {

	// General components
	struct Transform {
		glm::vec3 position;                                                   // The position relative to the parent
		glm::quat rotation = glm::angleAxis(0.0f, glm::vec3(0.0f,0.0f,1.0f)); // The rotation relative to the parent
		glm::vec3 scale    = glm::vec3(1.0f);                                 // The scale relative to the parent

		// The parent transform, if set to a invalid id then the position, rotation, and scale are in word space
		EntityID parent = InvalidEntityID;

		// Returns a Transform thats in world space
		Transform ApplyParent(ref<ECS> ecs) const;
		glm::mat4 GetRelativeMatrix() const;
		glm::mat4 GetWorldMatrix(ref<ECS> ecs) const;
	};

	// Render components
	struct ColoredQuad {
		glm::vec3 tint = glm::vec3(1.0f);
	};
	struct TexturedQuad {
		glm::vec3 tint = glm::vec3(1.0f);
		ref<Renderer::Texture> texture;
	};

	struct ColoredCircle {
		glm::vec3 tint = glm::vec3(1.0f);
	};
	struct TexturedCircle {
		glm::vec3 tint = glm::vec3(1.0f);
		ref<Renderer::Texture> texture;
	};

}