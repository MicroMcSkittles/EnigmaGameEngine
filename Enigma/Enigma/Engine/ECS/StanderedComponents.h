#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/EntityComponentSystem.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Engine {
		namespace ECS {

			class Transform : public Component {
			public:
				COMP_DEF(Transform);

				Transform(
					const glm::vec3& position = { 0,0,0 },
					const glm::vec3& rotation = { 0,0,0 },
					const glm::vec3& scale = { 1,1,1 },
					Transform* parentTransform = nullptr
				) : m_Position(position), m_Rotation(rotation),
					m_Scale(scale), m_ParentTransform(parentTransform) { }

				Transform*& GetParentTransform() { return m_ParentTransform; }

				// TODO: add world and relitive functions. Impl parent transform
				glm::vec3& GetPosition() { return m_Position; }
				glm::vec3& GetRotation() { return m_Rotation; }
				glm::vec3& GetScale() { return m_Scale; }

			private:
				Transform* m_ParentTransform;

				glm::vec3 m_Position;
				glm::vec3 m_Rotation; // TODO: learn how quaterniens work
				glm::vec3 m_Scale;
			};

			class Tag : public Component {
			public:
				COMP_DEF(Tag);
				Tag(const std::string& tag) : m_Tag(tag) { }

				std::string& GetTag() { return m_Tag; }

			private:
				std::string m_Tag;
			};

		}
	}
}