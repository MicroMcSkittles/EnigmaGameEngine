#include "Engine/ECS/Component.h"
#include "Engine/ECS/EntityComponentSystem.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Enigma {
	namespace Engine {
		namespace ECS {

			Transform Transform::ApplyParent()
			{
				if (!relative) return *this;
				if (parentTransform == Core::ID::InvalidID()) return *this;

				Transform parent = ECS::GetPool<Transform>()->Get(parentTransform).ApplyParent();

				glm::mat4 parentRotationMatrix = glm::mat4(1.0f);
				parentRotationMatrix = glm::rotate(parentRotationMatrix, parent.rotation.z, { 0,0,1 });
				parentRotationMatrix = glm::rotate(parentRotationMatrix, parent.rotation.y, { 0,1,0 });
				parentRotationMatrix = glm::rotate(parentRotationMatrix, parent.rotation.x, { 1,0,0 });

				glm::mat4 parentTransformMatrix = glm::mat4(1.0f);
				parentTransformMatrix = glm::translate(parentTransformMatrix, parent.position);
				parentTransformMatrix = parentTransformMatrix * parentRotationMatrix;
				parentTransformMatrix = glm::scale(parentTransformMatrix, parent.scale);

				Transform rslt;
				rslt.position = parentTransformMatrix * glm::vec4(position, 1.0f);
				rslt.rotation = rotation + parent.rotation;
				rslt.scale = scale * parent.scale;

				return rslt;
			}

		}
	}
}