#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Engine/ECS/Components.h"
#include <vector>
#include <math.h>
#include <glm/glm.hpp>

namespace Enigma::Engine::Physics {

	struct RidgidBody2D {
		glm::vec2 linearVelocity;
		
		glm::mat3 rotationMatrix; // TODO: fix the quaturnion stuff to remove this
		f32 angularVelocity;
	};

}