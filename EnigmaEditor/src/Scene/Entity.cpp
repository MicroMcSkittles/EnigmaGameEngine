#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "EditorImGui.h"
#include "EditorEvents.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Engine/Physics/PhysicsComponents.h>

#include <tuple>
#include <imgui.h>

using namespace Enigma::Engine::ECS;

namespace Enigma::Editor {
	Entity::Entity() : m_EntityID(Engine::ECS::InvalidEntityID), m_Scene(nullptr) { }
	Entity::Entity(Engine::ECS::EntityID entityID, Scene* scene) : m_EntityID(entityID), m_Scene(scene) { }
	
	bool Entity::operator==(const Entity& other) {
		return (m_EntityID == other.m_EntityID) && (m_Scene == other.m_Scene);
	}
	bool Entity::operator!=(const Entity& other) {
		return (m_EntityID != other.m_EntityID) || (m_Scene != other.m_Scene);
	}

	EntityMetaData& Entity::GetMetaData()
	{
		return GetComponent<EntityMetaData>();
	}

	bool Entity::Valid() const
	{
		if (m_EntityID == Engine::ECS::InvalidEntityID) return false;
		if (m_Scene == nullptr) return false;

		if (!HasComponent<EntityMetaData>()) return false;

		return true;
	}

	bool TransformGui(Entity entity) {
		Transform& transform = entity.GetComponent<Transform>();
		EntityMetaData& metaData = entity.GetMetaData();

		// TODO: add relative and degree controlls
		
		glm::vec3 originalEulerAngles = metaData.eulerAngles;
		bool edited = false;

		// Show input guis
		if (EditorGui::InputVec3("Position", transform.position))   edited = true;
		if (EditorGui::InputVec3("Rotation", metaData.eulerAngles)) edited = true;
		if (EditorGui::InputVec3("Scale", transform.scale, 1.0f))   edited = true;

		// Update quaternion if the euler angles were changed
		if (metaData.eulerAngles != originalEulerAngles) {
			transform.rotation = glm::quat(glm::radians(metaData.eulerAngles));
		}

		return edited;
	}
	bool RidgidBody2DGui(Entity entity) {
		Engine::Physics::RidgidBody2D& ridgidBody2D = entity.GetComponent<Engine::Physics::RidgidBody2D>();

		EditorGui::InputVec2("Linear Velocity", ridgidBody2D.linearVelocity, 0.0f, 125.0f);
		EditorGui::InputFloat("Angular Velocity", ridgidBody2D.angularVelocity, 0.0f, 125.0f);

		return false;
	}
	bool ColoredQuadGui(Entity entity) {
		ColoredQuad& coloredQuad = entity.GetComponent<ColoredQuad>();

		bool edited = false;

		// Show input guis
		edited = EditorGui::InputColor("Color", coloredQuad.tint, 1.0f);

		return edited;
	}

	// Gui implementation
	template <typename... Types>
	class ComponentGuiImpl {
	private:
		

		template <typename Comp>
		static void UndoRedoComponentAction(Entity entity, Comp other, bool* started) {
			entity.GetComponent<Comp>() = other;
			*started = false;
		}
		template <typename Comp>
		static void CreateComponentActionEvent(Entity entity, const std::string& name, Comp component, Comp original, bool* started) {
			Action action;
			action.undoFunc = std::bind(UndoRedoComponentAction<Comp>, entity, original, started);
			action.redoFunc = std::bind(UndoRedoComponentAction<Comp>, entity, component, started);
			action.name = "Edited component ( " + name + " ) of entity \"" + entity.GetMetaData().name + "\"";

			Event::NewAction e(action);
			Core::Application::EventCallback(e);
		}
		
		template <typename Comp>
		static void AddComponent(Entity entity, Comp component) {
			entity.CreateComponent<Comp>(component);
		}
		template <typename Comp>
		static void RemoveComponent(Entity entity) {
			entity.RemoveComponent<Comp>();
		}
		template <typename Comp>
		static void CreateRemovedComponentActionEvent(Entity entity, const std::string& name) {
			Action action;
			action.undoFunc = std::bind(AddComponent<Comp>, entity, entity.GetComponent<Comp>());
			action.redoFunc = std::bind(RemoveComponent<Comp>, entity);
			action.name = "Removed component ( " + name + " ) from entity \"" + entity.GetMetaData().name + "\"";
	
			Event::NewAction e(action);
			Core::Application::EventCallback(e);
		}
		template <typename Comp>
		static void CreateAddedComponentActionEvent(Entity entity, const std::string& name) {
			Action action;
			action.undoFunc = std::bind(RemoveComponent<Comp>, entity);
			action.redoFunc = std::bind(AddComponent<Comp>, entity, entity.GetComponent<Comp>());
			action.name = "Created component ( " + name + " ) in entity \"" + entity.GetMetaData().name + "\"";

			Event::NewAction e(action);
			Core::Application::EventCallback(e);
		}

		template<typename T, typename Func>
		static void ShowComponentGui(const std::string& name, Entity entity, Func function) {
			if (!entity.HasComponent<T>()) return;

			// Configure component dropdown
			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_AllowOverlap;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags |= ImGuiTreeNodeFlags_Framed;

			// Open component dropdown
			u64 componentHash = ECS::ComponentHasher<T>::Hash();
			ImGui::PushID(componentHash);
			bool open = ImGui::TreeNodeEx(name.c_str(), flags);
		
			// Show component settings button
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::SameLine(ImGui::GetWindowWidth() - 24.0f);
			if (ImGui::Button("...", ImVec2(20, 24))) {
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			// Show component settings popup
			bool removed = false;
			if (ImGui::BeginPopup("ComponentSettings")) {

				// Show remove option for any component that isn't a transform
				if (componentHash != ECS::ComponentHasher<Transform>::Hash()) {
					if (ImGui::MenuItem("Remove")) {
						CreateRemovedComponentActionEvent<T>(entity, name);
						entity.RemoveComponent<T>();
						removed = true;
					}
				}

				ImGui::EndPopup();
			}

			// End if removed
			if (removed) {
				if (open) ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			if (open) {

				if constexpr (std::is_invocable_v<Func, Entity>) {

					// Store original component data
					static bool started = false;
					static Entity last;
					static T original;
					if (last != entity) {
						started = false;
					}
					if (!started) {
						started = true;
						last = entity;
						original = entity.GetComponent<T>();
					}

					// Create a action event if component was edited
					if (function(entity)) {
						started = false;
						CreateComponentActionEvent(entity, name, entity.GetComponent<T>(), original, &started);
						original = entity.GetComponent<T>();
					}
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

		template <typename Tuple, u64... Indices>
		static auto ComponentSelectorsImpl(Tuple& t, std::index_sequence<Indices...>) {
			return std::vector<std::function<bool(Entity, const std::string&)>> {
				[&t](Entity entity, const std::string& name) {
					if (entity.HasComponent<std::tuple_element_t<Indices, std::tuple<Types...>>>()) return false;
					if (ImGui::Selectable(name.c_str())) {
						entity.CreateComponent<std::tuple_element_t<Indices, std::tuple<Types...>>>();
						CreateAddedComponentActionEvent< std::tuple_element_t<Indices, std::tuple<Types...>>>(entity, name);
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
		static std::vector<std::function<bool(Entity)>> s_UIFunctions;
	};

	using ComponentGui = ComponentGuiImpl<
		Transform,
		ColoredQuad,
		Engine::Physics::RidgidBody2D
	>;

	std::vector<std::string> ComponentGui::s_Names = {
		"Transform",
		"Colored Quad",
		"Ridgid Body 2D"
	};

	std::vector<std::function<bool(Entity)>> ComponentGui::s_UIFunctions = {
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
		if (!m_Entity) return;
		ImGui::PushID(m_Entity.GetID());
		EntityMetaData& metaData = m_Entity.GetMetaData();

		std::string uuid = m_Entity.GetUUID();

		ImGui::Text("%s | Entity ID: %u | UUID: %s", metaData.name.c_str(), m_Entity.GetID(), uuid.c_str());

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