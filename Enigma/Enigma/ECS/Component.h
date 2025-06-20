#pragma once
#include "Core/IdHandler.h"
#include "Core/Event/Event.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace ECS {
		enum class ComponentType {
			None = 0,
			Transform,
			Render2D,
			Camera,
			Script,
			Collider2D
		};

// Used to make creating new components easier
#define COMP_DEF(type) static ComponentType StaticGetType() { return ComponentType::type; } \
		static std::string StaticGetName() { return #type; }\
		static Core::ID CreateID(type* value) { return s_IDHandler.Create(value); }\
		static std::vector<type*>& GetList() { return s_IDHandler.GetData(); }\
		static type* Get(Core::ID& id) { return s_IDHandler.Get(id); }\
		static type* GetGlobal(Core::ID& id) { return (type*)s_GlobalIDHandler.Get(id); }\
		static bool IsValid(Core::ID& id) { return s_IDHandler.IsValid(id); }\
		static Core::IDHandler<type>& GetHandler() { return s_IDHandler; } \
		virtual void Delete() override { s_IDHandler.Delete(m_ID); s_GlobalIDHandler.Delete(m_GlobalID); m_ParentID = Core::ID::InvalidID(); } \
private:\
		inline static Core::IDHandler<type> s_IDHandler;\
public:

		class Component {
		public:
			Component() { 
				m_ParentID = Core::ID::InvalidID();
				m_GlobalID = s_GlobalIDHandler.Create(this);
				m_ID = Core::ID::InvalidID();
			}

			static Component* GetGlobal(Core::ID id) {
				return s_GlobalIDHandler.Get(id);
			}
			virtual void Delete() = 0;

			void SetParentID(Core::ID id) { m_ParentID = id; }
			Core::ID GetParentID() { return m_ParentID; }

			Core::ID GetID() { return m_ID; }
			Core::ID GetGlobalID() { return m_GlobalID; }

		protected:
			Core::ID m_ParentID;
			Core::ID m_GlobalID; // Every component gets regestered in a list with every other component with m_GlobalID
			Core::ID m_ID;

		protected:
			inline static Core::IDHandler<Component> s_GlobalIDHandler;
		};

		class Transform : public Component {
		public:
			COMP_DEF(Transform);
			Transform(const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
				const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f },
				const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, 
				Core::ID parentTransform = Core::ID::InvalidID())
				: m_Position(position), m_Rotation(rotation),
				m_Scale(scale), m_ParentTransform(parentTransform) 
			{ 
				m_ID = CreateID(this);
			}

			void SetPartentTransorm(Transform* transform) {
				m_ParentTransform = transform->m_ID;
			}
			Transform* GetParentTransform() { 
				if (!s_IDHandler.IsValid(m_ParentTransform)) return nullptr;
				return Get(m_ParentTransform); 
			}

			glm::vec3& GetPosition() { return m_Position; }
			glm::vec3& GetRotation() { return m_Rotation; }
			glm::vec3& GetScale() { return m_Scale; }

		private:
			Core::ID m_ParentTransform;

			glm::vec3 m_Position;
			glm::vec3 m_Rotation;
			glm::vec3 m_Scale;
		};

		class Script : public Component {
		public:
			COMP_DEF(Script);
			Script() {
				m_ID = CreateID(this);
				m_Started = false;
			}

			virtual void Start() = 0;
			virtual void Update(float deltaTime) = 0;
			virtual void Shutdown() = 0;

			virtual void OnEvent(Core::Event& e) { }

			bool& Started() { return m_Started; }

		private:
			bool m_Started;
		};
	}
}