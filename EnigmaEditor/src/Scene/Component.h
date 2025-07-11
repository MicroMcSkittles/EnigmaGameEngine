#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Component.h>
#include <Enigma/Engine/ECS/RenderComponent.h>
#include <Enigma/Core/Core.h>

#include "Panel/InspectorPanel.h"
#include "Serialization/JSON.h"

namespace Enigma {
	namespace Editor {

		class Entity;

		static const char* s_ComponentNames[] = {
			"Tag",
			"Transform",
			"Render2D"
		};
		static std::map<std::string, Engine::ECS::ComponentType> s_ComponentTypeMap = {
			{ "Tag",       Engine::ECS::ComponentType::Tag },
			{ "Transform", Engine::ECS::ComponentType::Transform },
			{ "Render2D",  Engine::ECS::ComponentType::Render2D },
		};

		class ComponentContext {
		public:
			virtual ~ComponentContext() { }
			virtual void ImGui() = 0;
		};

		class TagContext : public ComponentContext {
		public:
			TagContext(Entity* parent);

			virtual void ImGui() override;

		private:
			Entity* m_Parent;
			Engine::ECS::Tag* m_Component;
		};
		void SerializeTag(Engine::ECS::Tag& component, JSON::DataTreeNode& dataTree);
		void LoadTag(Engine::ECS::Tag& component, JSON::DataTreeNode& dataTree);

		class TransformContext : public ComponentContext {
		public:
			TransformContext(Entity* parent);

			virtual void ImGui() override;

		private:
			Entity* m_Parent;
			Engine::ECS::Transform* m_Component;
		};
		void SerializeTransform(Engine::ECS::Transform& component, JSON::DataTreeNode& dataTree);
		void LoadTransform(Engine::ECS::Transform& component, JSON::DataTreeNode& dataTree);

		class Render2DContext : public ComponentContext {
		public:
			Render2DContext(Entity* parent);

			virtual void ImGui() override;

		private:
			Entity* m_Parent;
			Engine::ECS::Render2D* m_Component;
		};
		void SerializeRender2D(Engine::ECS::Render2D& component, JSON::DataTreeNode& dataTree);
		void LoadRender2D(Engine::ECS::Render2D& component, JSON::DataTreeNode& dataTree);
	}
}