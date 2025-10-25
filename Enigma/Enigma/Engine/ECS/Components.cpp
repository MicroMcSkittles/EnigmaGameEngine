#include "Enigma/Engine/ECS/Components.h"
#include "Enigma/Core/Core.h"

namespace Enigma::Engine::ECS{

	Transform Transform::ApplyParent(ref<ECS> ecs) const
	{
		// Return the unmodifed data if there is no parent
		if (parent == InvalidEntityID) return *this;

		LOG_WARNING("Transform Transform::ApplyParent() not impl yet!!");
		return *this;
	}

	glm::mat4 Transform::GetRelativeMatrix() const
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = model * glm::mat4_cast(rotation);
		model = glm::scale(model, scale);

		return model;
	}

	glm::mat4 Transform::GetWorldMatrix(ref<ECS> ecs) const
	{
		Transform world = ApplyParent(ecs);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, world.position);
		model = model * glm::mat4_cast(world.rotation);
		model = glm::scale(model, world.scale);

		return model;
	}

}