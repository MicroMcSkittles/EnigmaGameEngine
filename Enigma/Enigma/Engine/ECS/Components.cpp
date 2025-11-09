#include "Enigma/Engine/ECS/Components.h"
#include "Enigma/Core/Core.h"

namespace Enigma::Engine::ECS{

	Transform Transform::ApplyParent(ref<ECS> ecs) const
	{
		// Return the unmodifed data if there is no parent
		if (parent == InvalidEntityID) return *this;

		//LOG_WARNING("Transform Transform::ApplyParent() not impl yet!!");
		Transform result;
		Transform parentTransform = ecs->GetComponent<Transform>(parent).ApplyParent(ecs);

		glm::mat4 parentModel = parentTransform.GetRelativeMatrix();

		result.position = parentModel * glm::vec4(position, 1.0f);
		result.rotation = rotation    * parentTransform.rotation;
		result.scale    = scale       * parentTransform.scale;

		return result;
	}

	glm::mat4 Transform::GetRelativeMatrix() const
	{
		glm::mat4 result = glm::mat4(1.0f);
		result = glm::translate(result, position);
		result = result * glm::mat4_cast(rotation);
		result = glm::scale(result, scale);

		return result;
	}

	glm::mat4 Transform::GetWorldMatrix(ref<ECS> ecs) const
	{
		if (parent == InvalidEntityID) return GetRelativeMatrix();

		Transform parentTransform = ecs->GetComponent<Transform>(parent).ApplyParent(ecs);
		
		glm::mat4 result = GetRelativeMatrix();
		result = parentTransform.GetWorldMatrix(ecs) * result;

		return result;
	}

}