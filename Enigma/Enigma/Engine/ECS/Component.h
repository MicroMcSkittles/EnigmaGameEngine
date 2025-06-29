#pragma once
#include "Core/IdHandler.h"

namespace Enigma {
	namespace Engine {
		namespace ECS {

			enum class ComponentType {
				None = 0,
				Transform,
				Tag,
				Render2D
			};

			class Component;

			class ComponentList {
			public:
				virtual Core::ID Create(Component* component) = 0;
				virtual void Delete(Core::ID id) = 0;
				virtual Component* Get(Core::ID id) = 0;
				virtual bool Exists(Core::ID id) = 0;
			};

			class Component {
			public:
				Component() {
					m_ParentEntityID = Core::ID::InvalidID();
					m_ID = Core::ID::InvalidID();
				}

				void SetParentEntityID(Core::ID id) { m_ParentEntityID = id; }

				Core::ID GetParentEntityID() { return m_ParentEntityID; }
				Core::ID GetID() { return m_ID; }

			private:
				Core::ID m_ParentEntityID;
				Core::ID m_ID;
			};

#define COMP_DEF(type) \
public:\
	class List : public ComponentList {\
	public:\
		virtual Core::ID Create(Component* component) override { return m_IDHandler.Create((type*)component); }\
		virtual void Delete(Core::ID id) override { m_IDHandler.Delete(id); }\
		virtual Component* Get(Core::ID id) override { return (Component*)m_IDHandler.Get(id); }\
		virtual bool Exists(Core::ID id) override { return m_IDHandler.IsValid(id); }\
		std::vector<type*>& GetList() { return m_IDHandler.GetData(); }\
	private:\
		Core::IDHandler<type> m_IDHandler;\
	};\
	static ComponentType GetStaticType() { return ComponentType::type; }\
	static std::string GetStaticName() { return #type; } \
	Entity* GetParentEntity() { return ECS::GetEntity(GetParentEntityID()); }
			\

		}
	}
}