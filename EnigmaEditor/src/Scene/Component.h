#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Component.h>
#include <Enigma/Engine/ECS/RenderComponent.h>
#include <Enigma/Core/Core.h>

#include <imgui.h>

#include "Panel/InspectorPanel.h"
#include "Serialization/JSON.h"
#include "Assets/TextureAsset.h"

namespace Enigma {
	namespace Editor {

		class Entity;

		static std::string s_ComponentNames[] = {
			"Tag",
			"Transform",
			"Render2D"
		};
		static std::map<std::string, Engine::ECS::ComponentType> s_ComponentTypeMap = {
			{ "Tag",       Engine::ECS::ComponentType::Tag },
			{ "Transform", Engine::ECS::ComponentType::Transform },
			{ "Render2D",  Engine::ECS::ComponentType::Render2D },
		};

		class EditorComponentInterface {
		public:
			virtual ~EditorComponentInterface() { }

			virtual void InspectorImGui() = 0;
			virtual Engine::ECS::ComponentType GetType() = 0;

			virtual void Save(JSON::DataTreeNode& dataTree) = 0;
			virtual void Load(JSON::DataTreeNode& dataTree) = 0;
		};

		template<typename T>
		class EditorComponent : public EditorComponentInterface {
		public:
			EditorComponent(Entity* entity) {
				m_Parent = entity;
				Engine::ECS::ECS::MakeCurrent(m_Parent->scene->GetECS());
				if (Engine::ECS::ECS::GetEntity(m_Parent->entityID).components.count(T::GetType()))
					m_ComponentID = Engine::ECS::ECS::GetComponentID<T>(m_Parent->entityID);
				else {
					Engine::ECS::ECS::AddComponent<T>(m_Parent->entityID);
					m_ComponentID = Engine::ECS::ECS::GetComponentID<T>(m_Parent->entityID);
				}
			}
			virtual ~EditorComponent() { }

			static Engine::ECS::ComponentType GetStaticType() { return T::GetType(); }
			static const char* GetStaticName() { return T::GetName(); }

			T& GetComponent() {
				Engine::ECS::ECS::MakeCurrent(m_Parent->scene->GetECS());
				return Engine::ECS::ECS::GetPool<T>()->Get(m_ComponentID);
			}

			virtual void InspectorImGui() {
				ImGui::PushID((void*)m_Parent);
				if (!ImGui::TreeNode(T::GetName())) {
					ImGui::PopID();
					return;
				}

				InspectorImGuiImpl(GetComponent());

				ImGui::TreePop();
				ImGui::PopID();
			}
			virtual Engine::ECS::ComponentType GetType() override { return T::GetType(); }

			virtual void Save(JSON::DataTreeNode& dataTree) = 0;
			virtual void Load(JSON::DataTreeNode& dataTree) = 0;

		protected:
			virtual void InspectorImGuiImpl(T& component) = 0;

		protected:
			Entity* m_Parent;
			Core::ID m_ComponentID;
		};

		class EditorTag : public EditorComponent<Engine::ECS::Tag> {
		public:
			EditorTag(Entity* entity) : EditorComponent(entity) { }

			virtual void Save(JSON::DataTreeNode& dataTree) override;
			virtual void Load(JSON::DataTreeNode& dataTree) override;

		protected:
			virtual void InspectorImGuiImpl(Engine::ECS::Tag& component) override;
		};

		class EditorTransform : public EditorComponent<Engine::ECS::Transform> {
		public:
			EditorTransform(Entity* entity) : EditorComponent(entity) { }

			virtual void Save(JSON::DataTreeNode& dataTree) override;
			virtual void Load(JSON::DataTreeNode& dataTree) override;

		protected:
			virtual void InspectorImGuiImpl(Engine::ECS::Transform& component) override;
		};

		class EditorRender2D : public EditorComponent<Engine::ECS::Render2D> {
		public:
			EditorRender2D(Entity* entity) : EditorComponent(entity), m_TextureAsset(nullptr) { }
			~EditorRender2D();

			virtual void Save(JSON::DataTreeNode& dataTree) override;
			virtual void Load(JSON::DataTreeNode& dataTree) override;

		protected:
			virtual void InspectorImGuiImpl(Engine::ECS::Render2D& component) override;

		private:
			TextureAsset* m_TextureAsset;
		};
	}
}