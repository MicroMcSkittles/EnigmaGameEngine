#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "EditorImGui.h"

#include <Enigma/Engine/Physics/PhysicsComponents.h>

#include <tuple>
#include <imgui.h>

namespace Enigma::Editor {
	Entity::Entity() : m_EntityID(Engine::ECS::InvalidEntityID), m_Scene(nullptr) {

	}
	Entity::Entity(Engine::ECS::EntityID entityID, Scene* scene) : m_EntityID(entityID), m_Scene(scene) {

	}
	bool Entity::Valid() const
	{
		if (m_EntityID == Engine::ECS::InvalidEntityID) return false;
		if (m_Scene == nullptr) return false;

		if (!HasComponent<EntityMetaData>()) return false;

		return true;
	}

	void TransformGui(Entity entity) {
		Engine::ECS::Transform& transform = entity.GetComponent<Engine::ECS::Transform>();
		EntityMetaData& metaData = entity.GetComponent<EntityMetaData>();

		// TODO: add relative and degree controlls

		EditorGui::InputVec3("Position", transform.position);

		if (EditorGui::InputVec3("Rotation", metaData.eulerAngles)) {
			transform.rotation = glm::rotate(glm::quat(), glm::radians(metaData.eulerAngles));
		}

		EditorGui::InputVec3("Scale", transform.scale, 1.0f);
	}

	void RidgidBody2DGui(Entity entity) {
		Engine::Physics::RidgidBody2D& ridgidBody2D = entity.GetComponent<Engine::Physics::RidgidBody2D>();

		EditorGui::InputVec2("Linear Velocity", ridgidBody2D.linearVelocity, 0.0f, 125.0f);
		EditorGui::InputFloat("Angular Velocity", ridgidBody2D.angularVelocity, 0.0f, 125.0f);
	}

	void ColoredQuadGui(Entity entity) {
		Engine::ECS::ColoredQuad& coloredQuad = entity.GetComponent<Engine::ECS::ColoredQuad>();

		EditorGui::InputColor("Color", coloredQuad.tint, 1.0f);
	}

	// Gui implementation

	template<typename... Types>
	class ComponentGuiImpl {
	private:
		template <typename Tuple, u64... Indices>
		static auto ComponentSelectorsImpl(Tuple& t, std::index_sequence<Indices...>) {
			return std::vector<std::function<bool(Entity, const std::string&)>> {
				[&t](Entity entity, const std::string& name) { 
					if (entity.HasComponent<std::tuple_element_t<Indices, std::tuple<Types...>>>()) return false;
					if (ImGui::Selectable(name.c_str())) {
						entity.CreateComponent<std::tuple_element_t<Indices, std::tuple<Types...>>>();
						return true;
					}
					return false;
				}...
			};

		}
		template <typename Tuple>
		static auto ComponentSelectors(Tuple t) {
			return ComponentSelectorsImpl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
		}

		template<typename T, typename Func>
		static void ShowComponentGui(const std::string& name, Entity entity, Func function) {
			if (!entity.HasComponent<T>()) return;

			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_AllowOverlap;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags |= ImGuiTreeNodeFlags_Framed;

			u64 componentHash = Engine::ECS::ECS::ComponentHasher<T>::Hash();
			ImGui::PushID(componentHash);
			bool open = ImGui::TreeNodeEx(name.c_str(), flags);
		
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::SameLine(ImGui::GetWindowWidth() - 24.0f);
			if (ImGui::Button("...", ImVec2(20, 24))) {
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			bool removed = false;
			if (ImGui::BeginPopup("ComponentSettings")) {

				if (componentHash != Engine::ECS::ECS::ComponentHasher<Engine::ECS::Transform>::Hash()) {
					if (ImGui::MenuItem("Remove")) {
						entity.RemoveComponent<T>();
						removed = true;
					}
				}

				ImGui::EndPopup();
			}

			if (removed) {
				if (open) ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			if (open) {

				if constexpr (std::is_invocable_v<Func, Entity>) {
					function(entity);
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <typename Tuple, u64... Indices>
		static auto ComponentEditorsImpl(Tuple& t, std::index_sequence<Indices...>) {
			return std::vector<std::function<void(Entity, const std::string&)>> {
				[&t](Entity entity, const std::string& name) {
					ShowComponentGui<std::tuple_element_t<Indices, std::tuple<Types...>>>(name, entity, s_UIFunctions[Indices]);
				}...
			};

		}
		template <typename Tuple>
		static auto ComponentEditors(Tuple t) {
			return ComponentEditorsImpl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
		}

	public:
		static bool AddComponentGui(Entity entity) {
			ImGui::Text("Add Component");
			if (!ImGui::BeginListBox("##AddComponentListBox")) return false;

			auto selectors = ComponentSelectors(std::tuple<Types...>());

			for (u64 i = 0; i < sizeof...(Types); ++i) {
				if (selectors[i](entity, s_Names[i])) {
					ImGui::EndListBox();
					return true;
				}
			}

			ImGui::EndListBox();
			return false;
		}
		static void ShowComponents(Entity entity) {
			auto editors = ComponentEditors(std::tuple<Types...>());

			for (u64 i = 0; i < sizeof...(Types); ++i) {
				editors[i](entity, s_Names[i]);
				ImGui::Spacing();
			}
		}

	private:
		static std::vector<std::string> s_Names;
		static std::vector<std::function<void(Entity)>> s_UIFunctions;
	};

	using ComponentGui = ComponentGuiImpl<
		Engine::ECS::Transform,
		Engine::ECS::ColoredQuad,
		Engine::Physics::RidgidBody2D
	>;

	std::vector<std::string> ComponentGui::s_Names = {
		"Transform",
		"Colored Quad",
		"Ridgid Body 2D"
	};

	std::vector<std::function<void(Entity)>> ComponentGui::s_UIFunctions = {
		TransformGui,
		ColoredQuadGui,
		RidgidBody2DGui
	};

	EntityInspectorContext::EntityInspectorContext(Entity entity)
	{
		m_Entity = entity;
	}
	void EntityInspectorContext::ShowGui()
	{
		if (!m_Entity.Valid()) return;
		ImGui::PushID(m_Entity.GetID());
		EntityMetaData& metaData = m_Entity.GetComponent<EntityMetaData>();
		ImGui::Text("%s | Entity ID: %u | UUID: %u", metaData.name.c_str(), m_Entity.GetID(), 12385710);

		ComponentGui::ShowComponents(m_Entity);

		if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponentPopup");

		if (ImGui::BeginPopup("AddComponentPopup")) {

			if (ComponentGui::AddComponentGui(m_Entity)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
	
}