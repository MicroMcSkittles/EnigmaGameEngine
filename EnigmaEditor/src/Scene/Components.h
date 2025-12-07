#pragma once
#include "Scene/Entity.h"

namespace Enigma::Editor {

	struct SceneMetaData {
		Entity activeCamera;
	};

	// General scene data needed by the editor but not important in the final product
	struct EntityMetaData {
		std::string name;

		Entity parent;
		Core::SparseSet<Entity> children;
	};

	// Transform data needed by the editor but not important in the final product
	struct TransformMetaData {
		bool degrees = true; // Should inputs be in degrees or radians
		glm::vec3 eulerAngles = glm::vec3(0.0f); // Rotation stored as euler angles instead of a quaturnian
	};

}