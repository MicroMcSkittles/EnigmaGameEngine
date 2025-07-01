#pragma once
#include "Core/IdHandler.h"
#include <glm/glm.hpp>

namespace Enigma {
	namespace Engine {
		namespace ECS {

			enum class ComponentType {
				None = 0,
				Tag,
				Transform
			};

			class IComponentPool {
			public:
				virtual ~IComponentPool() { }
			};

			template<typename T>
			class ComponentPool : public IComponentPool {
			public:
				T& Get(Core::ID id) { return m_Data.Get(id); }
				Core::ID Add(T comp) { return m_Data.Create(comp); }
				void Remove(Core::ID id) { return m_Data.Delete(id); }
				std::vector<T>& GetData() { return m_Data.GetData(); }

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

				glm::vec3 position;
				glm::vec3 rotation;
				glm::vec3 scale = glm::vec3(1);

				Core::ID parentTransform;
			};
		}
	}
}