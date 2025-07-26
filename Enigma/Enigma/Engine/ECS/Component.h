#pragma once
#include "Enigma/Core/IdHandler.h"
#include <glm/glm.hpp>

namespace Enigma {
	namespace Engine {
		namespace ECS {

			enum class ComponentType {
				None = 0,
				Tag,
				Transform,
				Render2D,
				Camera
			};

			class ComponentPoolInterface {
			public:
				virtual ~ComponentPoolInterface() { }
				virtual void Remove(Core::ID id) = 0;
			};

			template<typename T>
			class ComponentPool : public ComponentPoolInterface {
			public:
				~ComponentPool() {
					m_Data.Clear();
				}

				T& Get(Core::ID id) { return m_Data.Get(id); }
				Core::ID Add(T comp) { return m_Data.Create(comp); }
				std::vector<T>& GetData() { return m_Data.GetData(); }
				virtual void Remove(Core::ID id) override { m_Data.Delete(id); }

			private:
				Core::IDHandler<T> m_Data;
			};

#define COMP_DEF(type) \
			static const char* GetName() { return #type; } \
			static ComponentType GetType() { return ComponentType::type; } \
			Core::ID parentEntity;
			
			struct Tag {
				COMP_DEF(Tag);

				std::string tag;
			};

			struct Transform {
				COMP_DEF(Transform);

				glm::vec3 position = glm::vec3(0);
				glm::vec3 rotation = glm::vec3(0);
				glm::vec3 scale = glm::vec3(1);

				// Is the transform relative to its parent
				bool relative = true;

				// The ID of the transforms parent transform
				Core::ID parentTransform = Core::ID::InvalidID();

				// Transforms the transform based on the parent transform
				Transform ApplyParent();
			};
		}
	}
}