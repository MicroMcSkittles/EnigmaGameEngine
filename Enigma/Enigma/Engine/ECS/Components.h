#pragma once
#include "Enigma/Engine/ECS/EntityComponentSystem.h"
#include "Enigma/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Enigma::Engine::ECS {

	// General components
	struct Transform {
		glm::vec3 position; // The position relative to the parent
		glm::quat rotation; // The rotation relative to the parent
		glm::vec3 scale;    // The scale relative to the parent

		// The parent transform, if set to a invalid id then the position, rotation, and scale are in word space
		EntityID parent = InvalidEntityID;

		// TODO: impl this function
		// Returns a Transform thats in world space
		Transform ApplyParent(ECS* ecs) const;
		glm::mat4 GetRelativeMatrix() const;
		glm::mat4 GetWorldMatrix(ECS* ecs) const;
	};

	// Render components
	struct ColoredQuad {
		glm::vec3 tint;
	};
	struct TexturedQuad {
		glm::vec3 tint;
		Renderer::Texture* texture;
	};

	struct ColoredCircle {
		glm::vec3 tint;
	};
	struct TexturedCircle {
		glm::vec3 tint;
		Renderer::Texture* texture;
	};

}