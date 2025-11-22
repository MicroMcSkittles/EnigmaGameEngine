#pragma once
#include "Scene/Scene.h"

namespace Enigma::Editor {

	class SceneSerializer {
	public:
		SceneSerializer(const ref<Scene>& scene);

		void Serialize(const std::string& filename);
		void SerializeBinary(const std::string& filename);

		bool Deserialize(const std::string& filename);
		bool DeserializeBinary(const std::string& filename);

	private:
		ref<Scene> m_Scene;
	};

}