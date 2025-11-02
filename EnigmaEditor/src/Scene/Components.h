#pragma once
#include "Scene/Entity.h"

namespace Enigma::Editor {

	// Data needed by the editor but not important in the final product
	struct EntityMetaData {
		std::string name;

		Entity parent;
		Core::SparseSet<Entity> children;
	};

}