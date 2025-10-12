#include "Enigma/Engine/ECS/Components.h"
#include "Enigma/Core/Core.h"

namespace Enigma::Engine::ECS{

	Transform Transform::ApplyParent(ECS* ecs)
	{
		// Return the unmodifed data if there is no parent
		if (parent == InvalidEntityID) return *this;

		LOG_WARNING("Transform Transform::ApplyParent() not impl yet!!");
		return *this;
	}

}