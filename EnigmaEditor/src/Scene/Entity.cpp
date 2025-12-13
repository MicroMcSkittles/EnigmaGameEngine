#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "EditorImGui.h"
#include "EditorEvents.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Engine/Physics/PhysicsComponents.h>
#include <Enigma/ImGui/EnigmaWidgets.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <tuple>

using namespace Enigma::Engine::ECS;

namespace Enigma::Editor {
	Entity::Entity() : m_EntityID(Engine::ECS::InvalidEntityID), m_Scene(nullptr) { }
	Entity::Entity(Engine::ECS::EntityID entityID, Scene* scene) : m_EntityID(entityID), m_Scene(scene) { }
	
	bool Entity::operator==(const Entity& other) { return (m_EntityID == other.m_EntityID) && (m_Scene == other.m_Scene); }
	bool Entity::operator!=(const Entity& other) { return (m_EntityID != other.m_EntityID) || (m_Scene != other.m_Scene); }

	EntityMetaData& Entity::GetMetaData() { return GetComponent<EntityMetaData>(); }

	bool Entity::Valid() const {
		if (m_EntityID == Engine::ECS::InvalidEntityID) return false;
		if (m_Scene == nullptr) return false;

		if (!HasComponent<EntityMetaData>()) return false;

		return true;
	}


	// Gui implementation ============
	void BlankMenuGui(Entity entity) { }

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
			action.name = "Edited component \"" + name + "\" of entity \"" + entity.GetMetaData().name + "\"";

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
			action.name = "Removed component \"" + name + "\" from entity \"" + entity.GetMetaData().name + "\"";

			Event::NewAction e(action);
			Core::Application::EventCallback(e);
		}
		template <typename Comp>
		static void CreateAddedComponentActionEvent(Entity entity, const std::string& name) {
			Action action;
			action.undoFunc = std::bind(RemoveComponent<Comp>, entity);
			action.redoFunc = std::bind(AddComponent<Comp>, entity, entity.GetComponent<Comp>());
			action.name = "Created component \"" + name + "\" in entity \"" + entity.GetMetaData().name + "\"";

			Event::NewAction e(action);
			Core::Application::EventCallback(e);
		}

		// this is cursed btw, like the rest of this class isn't :(
		template<typename T>
		static bool AreComponentsEqual(T& first, T& second) {

			constexpr u64 ComponentByteSize = sizeof(T);

			// Get pointers to the byte data of the components
			u8* firstBytes = reinterpret_cast<u8*>(&first);
			u8* secondBytes = reinterpret_cast<u8*>(&second);

			// Check each byte aganst each other
			for (u64 i = 0; i < ComponentByteSize; ++i) {
				if (*firstBytes++ != *secondBytes++) return false;
			}

			return true;
		}

		template<typename T, typename MenuFunc, typename Func>
		static void ShowComponentGui(const std::string& name, Entity entity, MenuFunc menuFunction, Func function) {
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
			u64 componentHash = ComponentHasher<T>::Hash();
			ImGui::PushID(componentHash);
			bool open = ImGui::TreeNodeEx(name.c_str(), flags);

			// Show component settings button
			ImGui::SameLine(ImGui::GetContentRegionMax().x - 20.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			bool optionsButton = ImGui::ImageButton(
				"ComponentSettingsOptionsButton",
				ImGui::ToImGuiTexture(EditorGui::GetIcon(EditorIcon_Menu)),
				ImVec2(24, 24)
			);
			if (optionsButton)ImGui::OpenPopup("ComponentSettings");
			ImGui::PopStyleVar();

			// Show component settings popup
			bool removed = false;
			if (ImGui::BeginPopup("ComponentSettings")) {

				// Show remove option for any component that isn't a transform
				if (componentHash != ComponentHasher<TransformComponent>::Hash()) {
					if (ImGui::MenuItem("Remove")) {
						CreateRemovedComponentActionEvent<T>(entity, name);
						entity.RemoveComponent<T>();
						removed = true;
					}
				}

				menuFunction(entity);

				ImGui::EndPopup();
			}

			// End if removed
			if (removed) {
				if (open) ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			// Exit if component isn't open
			if (!open) {
				ImGui::PopID();
				return;
			}

			if constexpr (std::is_invocable_v<Func, Entity>) {

				// Store original component data
				static bool started = false;
				static bool edited = false;
				static Entity last;
				static T original;

				T& component = entity.GetComponent<T>();

				// Update static vars if inspector was just opened
				if (last != entity) started = false;
				if (!started) {
					started = true;
					last = entity;
					original = component;
				}
				// Update original if component was edited by something other than the inspector
				else if (!edited && !AreComponentsEqual(original, component)) original = component;

				// Create a action event if component was edited
				if (function(entity)) {
					started = false;
					CreateComponentActionEvent(entity, name, component, original, &started);
					original = component;
				}

				// Check if component was edited by the inspector
				edited = !AreComponentsEqual(original, component);
			}

			ImGui::TreePop();

			ImGui::PopID();
		}

		template <typename Tuple, u64... Indices>
		static auto ComponentEditorsImpl(Tuple& t, std::index_sequence<Indices...>) {
			return std::vector<std::function<void(Entity, const std::string&)>> {
				[&t](Entity entity, const std::string& name) {
					ShowComponentGui<std::tuple_element_t<Indices, std::tuple<Types...>>>(name, entity, s_UIMenuFunctions[Indices], s_UIFunctions[Indices]);
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
					if (ImGui::MenuItem(name.c_str())) {
						entity.CreateComponent<std::tuple_element_t<Indices, std::tuple<Types...>>>();
						CreateAddedComponentActionEvent<std::tuple_element_t<Indices, std::tuple<Types...>>>(entity, name);
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

			auto selectors = ComponentSelectors(std::tuple<Types...>());

			for (u64 i = 0; i < sizeof...(Types); ++i) {
				if (selectors[i](entity, s_Names[i])) return true;
			}

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
		static std::vector<std::function<void(Entity)>> s_UIMenuFunctions;
	};
	// ===============================

	// General components ============
	bool TransformGui(Entity entity) {
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		TransformMetaData& metaData = entity.GetComponent<TransformMetaData>();

		// Angle values
		glm::vec3 originalEulerAngles = (metaData.degrees) ? glm::degrees(metaData.eulerAngles) : metaData.eulerAngles;
		glm::vec3 eulerAngles = originalEulerAngles;
		bool edited = false;

		// Show input guis
		if (EditorGui::InputVec3("Position", transform.position)) edited = true;
		if (EditorGui::InputVec3("Rotation", eulerAngles))        edited = true;
		if (EditorGui::InputVec3("Scale", transform.scale, 1.0f)) edited = true;

		// Update quaternion if the euler angles were changed
		if (eulerAngles != originalEulerAngles) {
			metaData.eulerAngles = (metaData.degrees) ? glm::radians(eulerAngles) : eulerAngles;
			transform.rotation = glm::quat(metaData.eulerAngles);
		}

		// Relative controls
		if (entity.GetMetaData().parent) {
			bool relative = (transform.parent != InvalidEntityID);
			if (EditorGui::CheckBox("Relative", relative)) {
				if (relative) transform.parent = entity.GetMetaData().parent.GetID();
				else transform.parent = InvalidEntityID;
				edited = true;
			}
		}

		return edited;
	}
	void TransformMenuGui(Entity entity) {
		TransformMetaData& metaData = entity.GetComponent<TransformMetaData>();
		ImGui::Checkbox("Degrees", &metaData.degrees);
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Show rotation in degrees");
			ImGui::EndTooltip();
		}
	}

	// Renderer components ===========
	bool QuadRendererGui(Entity entity) {
		QuadRendererComponent& quad = entity.GetComponent<QuadRendererComponent>();
		bool edited = false;

		// Show input guis
		if (EditorGui::InputColor("Color", quad.tint, 1.0f)) edited = true;

		return edited;
	}
	bool CircleRendererGui(Entity entity) {
		CircleRendererComponent& circle = entity.GetComponent<CircleRendererComponent>();
		bool edited = false;

		// Show input guis
		if (EditorGui::InputColor("Color",     circle.tint, 1.0f))              edited = true;
		if (EditorGui::InputFloat("Thickness", circle.thickness, 1.0f))         edited = true;
		if (EditorGui::InputFloat("Fade",      circle.fade, 0.005f, 100.0f, 4)) edited = true;
		
		return edited;
	}
	bool OrthographicCameraGui(Entity entity) {
		OrthographicCameraComponent& camera = entity.GetComponent<OrthographicCameraComponent>();

		bool edited = false;

		if (EditorGui::CheckBox("Fit To Screen", camera.fitToScreen)) edited = true;
		if (!camera.fitToScreen) {
			if (EditorGui::InputFloat("Left", camera.view.left))     edited = true;
			if (EditorGui::InputFloat("Right", camera.view.right))   edited = true;
			if (EditorGui::InputFloat("Top", camera.view.top))       edited = true;
			if (EditorGui::InputFloat("Bottom", camera.view.bottom)) edited = true;
		}
		if (EditorGui::InputFloat("Near",   camera.view.near))   edited = true;
		if (EditorGui::InputFloat("Far",    camera.view.far))    edited = true;
		if (EditorGui::InputFloat("Zoom",   camera.zoom))        edited = true;

		return edited;
	}

	// Physics components ============
	bool RidgidBody2DGui(Entity entity) {
		Engine::Physics::RidgidBody2D& ridgidBody2D = entity.GetComponent<Engine::Physics::RidgidBody2D>();

		bool edited = false;

		if (EditorGui::InputVec2("Linear Velocity", ridgidBody2D.linearVelocity, 0.0f, 125.0f)) edited = true;
		if (EditorGui::InputFloat("Angular Velocity", ridgidBody2D.angularVelocity, 0.0f, 125.0f)) edited = true;

		return edited;
	}
	void RidgidBody2DMenuGui(Entity entity) {
		bool degrees = false;
		ImGui::Checkbox("Degrees", &degrees);
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Show inputs in degrees");
			ImGui::EndTooltip();
		}
	}

	// TODO: find a better way to handle component guis
	using ComponentGui = ComponentGuiImpl<
		TransformComponent,
		OrthographicCameraComponent,
		QuadRendererComponent,
		CircleRendererComponent,
		Engine::Physics::RidgidBody2D
	>;

	std::vector<std::string> ComponentGui::s_Names = {
		"Transform",
		"Orthographic Camera",
		"Quad Renderer",
		"Circle Renderer",
		"Ridgid Body 2D"
	};
	std::vector<std::function<bool(Entity)>> ComponentGui::s_UIFunctions = {
		TransformGui,
		OrthographicCameraGui,
		QuadRendererGui,
		CircleRendererGui,
		RidgidBody2DGui
	};
	std::vector<std::function<void(Entity)>> ComponentGui::s_UIMenuFunctions = {
		TransformMenuGui,
		BlankMenuGui,
		BlankMenuGui,
		BlankMenuGui,
		RidgidBody2DMenuGui
	};

	EntityInspectorContext::EntityInspectorContext(Entity entity, ref<Scene> scene) {
		m_Entity = entity;
		m_Scene = scene;
	}
	void EntityInspectorContext::ShowGui() {
		if (!m_Entity) return;
		ImGui::PushID(m_Entity.GetID());

		HeaderGui();

		ImGui::Separator();

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

	void EntityInspectorContext::HeaderGui()
	{
		EntityMetaData& metaData = m_Entity.GetMetaData();

		// Get entity name and cap its length
		std::string entityName = metaData.name;
		constexpr u64 maxNameLength = 32;
		if (entityName.size() > maxNameLength) {
			entityName.resize(maxNameLength);
			memset(entityName.data() + maxNameLength - 3, '.', 3);
		}

		// Get IDs as strings
		std::string uuidString = m_Entity.GetUUID();
		std::string ecsIDString = std::to_string(m_Entity.GetID());

		bool renaming = false;
		std::string original;
		if (EditorGui::RenamableText(metaData.name, "EntityRenameBox", &renaming, &original)) {
			RenameEntityAction(m_Scene, m_Entity, metaData.name, original);
		}

		if (renaming) return;

		// Show IDs
		ImGui::SameLine();
		ImGui::TextDisabled("ECS ID: %s", ecsIDString.c_str());
		if (ImGui::BeginItemTooltip()) {
			ImGui::TextDisabled("UUID: %s", uuidString.c_str());
			ImGui::EndTooltip();
		}
	}
	
}