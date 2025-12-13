#include "Panels/SceneViewPanel.h"
#include "Scene/Components.h"
#include "EditorImGui.h"
#include "EditorEvents.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>

#include <Enigma/Engine/Input.h>
#include <Enigma/Engine/InputCodes.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <Enigma/ImGui/EnigmaWidgets.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Enigma::Editor {

	static void UndoRedoChangedTransform(Entity entity, Engine::ECS::TransformComponent transform) {
		entity.GetComponent<Engine::ECS::TransformComponent>() = transform;
		entity.GetComponent<TransformMetaData>().eulerAngles = glm::eulerAngles(transform.rotation);
	}
	static void ChangedTransformAction(Entity entity, const Engine::ECS::TransformComponent& original) {
		Action action;
		action.undoFunc = std::bind(UndoRedoChangedTransform, entity, original);
		action.redoFunc = std::bind(UndoRedoChangedTransform, entity, entity.GetComponent<Engine::ECS::TransformComponent>());
		action.name = "Edited component \"Transform\" of entity \"" + entity.GetMetaData().name + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}

	SceneViewPanel::SceneViewPanel(Core::ID windowID) : m_WindowID(windowID) 
	{
		m_Hovered      = false;
		m_Focused      = false;
		m_ShowGizmos   = true;
		m_GizmoHovered = false;
		m_UsingGizmo   = false;
		m_CameraSpeed  = 7.5f;
		m_ZoomSpeed    = 0.125f;
		m_MaxZoom      = 1.0f;
		m_MinZoom      = 100.0f;
		m_GizmoType    = ImGuizmo::OPERATION::TRANSLATE;
		m_EditorState  = EditorState_Editing;

		ref<Core::Window> window = Core::Application::GetWindow(m_WindowID);
		Core::Application::UseRenderAPI(window->GetAPI());

		// Create output frame texture
		Renderer::TextureConfig frameConfig;
		frameConfig.width = window->GetWidth();
		frameConfig.height = window->GetHeight();
		m_Frame = Renderer::Texture::Create(frameConfig);

		// Create surface
		m_Surface.position = { 0.0f, 0.0f };
		m_Surface.scale = { window->GetWidth(), window->GetHeight() };
		m_Surface.frame = m_Frame;

		// Create input context
		Engine::InputConfig inputConfig;
		inputConfig.surface = &m_Surface;
		inputConfig.window = window;
		m_InputContext = Engine::Input::Create(inputConfig);

		// Create camera
		Renderer::ViewBox view = Renderer::ViewBox::SurfaceViewBox(m_Surface);
		m_EditorCamera = Renderer::OrthographicCamera::Create(view, 4, { 0.0f,0.0f, view.far - 1.0f });
		m_RuntimeCamera = Renderer::OrthographicCamera::Create(view, 4, { 0.0f,0.0f, view.far - 1.0f });
		m_ActiveCamera = m_EditorCamera;

		m_RendererContext = Renderer::Renderer2D::Create(window->GetAPI(), m_Surface);
	}
	void SceneViewPanel::SetContext(ref<Scene> context) {
		m_Context = context;
		if (context == nullptr) m_RendererContext = nullptr;
	}
	void SceneViewPanel::SetSelected(Entity entity)
	{
		m_Selected = entity;
	}

	void SceneViewPanel::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);
		handler.Dispatch<Core::Keyboard>([&](Core::Keyboard& e) { OnKeyboard(e); return false; });
		handler.Dispatch<Core::MouseScroll>([&](Core::MouseScroll& e) { OnScroll(e); return false; });
		
		handler.Dispatch<Event::EntitySelected>([&](Event::EntitySelected& e) {
			m_Selected = e.GetEntity();
			return false;
		});
		handler.Dispatch<Event::SceneChange>([&](Event::SceneChange& e) {
			m_Selected = {};
			SetContext(e.GetScene());
			return false;
		});

		handler.Dispatch<Event::StartRuntime>([&](Event::StartRuntime& e) {
			m_EditorState = EditorState_Running;
			return false; 
		});
		handler.Dispatch<Event::PauseRuntime>([&](Event::PauseRuntime& e) { 
			m_EditorState = EditorState_Editing;
			return false; 
		});
	}
	void SceneViewPanel::OnScroll(Core::MouseScroll& e)
	{
		if (m_EditorState != EditorState_Editing) return;
		if (!Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight)) return;
		if (!m_Hovered) return;

		// Get camera zoom
		ref<Renderer::OrthographicCamera> orthoCamera = CastRef<Renderer::OrthographicCamera>(m_EditorCamera);
		f32 zoom = orthoCamera->GetZoom();

		// Calculate new zoom value
		f32 delta = e.GetY();
		delta *= -m_ZoomSpeed;
		zoom += delta;

		// Cap zoom
		zoom = std::max(zoom, m_MaxZoom);
		zoom = std::min(zoom, m_MinZoom);

		// Apply zoom to camera
		orthoCamera->SetZoom(zoom);
	}
	void SceneViewPanel::OnKeyboard(Core::Keyboard& e)
	{
		if (e.GetAction() != Engine::KeyCode::KeyPress) return;
		if (Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight)) return;
		if (!m_Focused) return;

		// TODO: custom key binds
		switch (e.GetKey()) {
		case Engine::KeyCode::KeyW: m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
		case Engine::KeyCode::KeyE: m_GizmoType = ImGuizmo::OPERATION::SCALE;     break;
		case Engine::KeyCode::KeyR: m_GizmoType = ImGuizmo::OPERATION::ROTATE;    break;
		}
	}
	
	void SceneViewPanel::Update(Engine::DeltaTime deltaTime)
	{
		Engine::Input::MakeCurrent(m_InputContext);

		// Handle camera controls
		if (!m_Hovered || !Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight) || m_EditorState != EditorState_Editing) return;

		// Get movement direction
		glm::vec2 moveDir = glm::vec2(0.0f);
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyW)) moveDir.y += 1.0f;
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyA)) moveDir.x += -1.0f;
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyS)) moveDir.y += -1.0f;
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyD)) moveDir.x += 1.0f;

		// Update camera position
		if (moveDir.x || moveDir.y) {
			moveDir = glm::normalize(moveDir);
			glm::vec3 cameraPos = m_EditorCamera->GetPosition();
			f32 zoom = CastRef<Renderer::OrthographicCamera>(m_EditorCamera)->GetZoom();
			f32 speed = m_CameraSpeed * (zoom / 4.0f);
			cameraPos += glm::vec3(moveDir * speed * (float)deltaTime, 0.0f);
			m_EditorCamera->SetPosition(cameraPos);
		}
	}

	void SceneViewPanel::ShowGui() {
		ImGui::Begin("Scene View");

		if (m_Context == nullptr) {
			ImGui::Text("No scene selected");
			ImGui::End();
			return;
		}

		// Get the size of ImGui window
		glm::vec2 region = FromImVec(ImGui::GetContentRegionAvail());
		m_Surface.position = FromImVec(ImGui::GetCursorScreenPos());

		// Make sure the texture is the right size
		if (m_Surface.scale != region && m_RendererContext != nullptr) {
			m_Surface.Resize(region.x, region.y);
			m_RendererContext->Resize(region.x, region.y);
			m_ActiveCamera->Resize(region.x, region.y);
		}

		m_TopWindowPosition = FromImVec(ImGui::GetCursorScreenPos());

		// Show texture
		ImGui::Image(m_Frame);

		m_BottomWindowPosition = m_TopWindowPosition + FromImVec(ImGui::GetContentRegionAvail());
		// Process window status
		m_Hovered = ImGui::IsItemHovered();
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && m_Hovered) {
			ImGui::SetWindowFocus();
		}
		m_Focused = ImGui::IsWindowFocused();

		// Gizmos
		if (m_Selected && m_GizmoType != -1) ShowGizmos();
		else m_GizmoHovered = false;

		// Show overlay windows
		ShowGizmoOverlayWindow();
		ShowRuntimeOverlayWindow();
		ShowSettingsOverlayWindow();

		// Process mouse input
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && m_Hovered && !m_GizmoHovered) {
			m_Selected = GetHoveredEntity();
		}
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_Hovered && !m_GizmoHovered) {
			if (m_Selected = GetHoveredEntity()) {
				Event::EntitySelected selectionEvent(m_Selected);
				Core::Application::EventCallback(selectionEvent);
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_Hovered && !m_GizmoHovered) {
			if (m_Selected = GetHoveredEntity()) ImGui::OpenPopup("EntityMenu");
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_GizmoHovered) {
			ImGui::OpenPopup("GizmoMenu");
		}

		// Show menu popups
		ShowEntityMenu();
		ShowGizmoMenu();

		ImGui::End();
	}
	void SceneViewPanel::ShowEntityMenu()
	{
		if (!ImGui::BeginPopup("EntityMenu")) return;
		
		// Get entity name and cap its length
		std::string entityName = m_Selected.GetMetaData().name;
		constexpr u64 maxNameLength = 16;
		if (entityName.size() > maxNameLength) {
			entityName.resize(maxNameLength);
			memset(entityName.data() + maxNameLength - 3, '.', 3);
		}

		std::string uuidString = m_Selected.GetUUID();
		std::string ecsIDString = std::to_string(m_Selected.GetID());

		// Show entity info
		ImGui::Text("%s", entityName.c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("ECS ID: %s", ecsIDString.c_str());
		if (ImGui::BeginItemTooltip()) {
			ImGui::TextDisabled("UUID: %s", uuidString.c_str());
			ImGui::EndTooltip();
		}

		ImGui::Separator();

		// Options
		if (ImGui::MenuItem("Remove")) {
			RemoveEntityAction(m_Context, m_Selected);
			m_Context->RemoveEntity(m_Selected);

			Event::EntitySelected e({});
			Core::Application::EventCallback(e);
		}

		ImGui::EndPopup();
	}
	
	void SceneViewPanel::ShowGizmoMenu()
	{
		if (!ImGui::BeginPopup("GizmoMenu")) return;
		if (ImGui::MenuItem("Transform", "W")) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		if (ImGui::MenuItem("Scale",     "E")) m_GizmoType = ImGuizmo::OPERATION::SCALE;
		if (ImGui::MenuItem("Rotate",    "R")) m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		ImGui::EndPopup();
	}
	void SceneViewPanel::ShowGizmos()
	{
		if (!m_ShowGizmos) return;
		// Configure ImGuizmo
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_Surface.position.x, m_Surface.position.y, m_Surface.scale.x, m_Surface.scale.y);

		// Camera
		const glm::mat4& cameraProj = m_ActiveCamera->GetProjection();
		const glm::mat4& cameraView = m_ActiveCamera->GetView();

		// Entity transform
		Engine::ECS::TransformComponent& transform = m_Selected.GetComponent<Engine::ECS::TransformComponent>();
		glm::mat4 transformWorldMatrix = transform.ApplyParent(m_Context->GetECS()).GetRelativeMatrix();//= transform.GetWorldMatrix(m_Context->GetECS());
		
		glm::mat4 deltaMatrix = glm::mat4(1.0f);

		ImGuizmo::Manipulate(
			glm::value_ptr(cameraView),
			glm::value_ptr(cameraProj),
			static_cast<ImGuizmo::OPERATION>(m_GizmoType),
			ImGuizmo::MODE::LOCAL,
			glm::value_ptr(transformWorldMatrix),
			glm::value_ptr(deltaMatrix)
		);

		// Process gizmo states
		m_GizmoHovered = ImGuizmo::IsOver();

		// Store the transform if the user just started to move the entity
		if (!m_UsingGizmo && ImGuizmo::IsUsing()) {
			m_OriginalTransform = transform;
		}
		// Create changed transform action when the user lets go
		else if (m_UsingGizmo && !ImGuizmo::IsUsing()) {
			ChangedTransformAction(m_Selected, m_OriginalTransform);
		}
		m_UsingGizmo = ImGuizmo::IsUsing();
		if (!m_UsingGizmo) return;

		// Process input
		glm::mat4 transformLocalMatrix = transformWorldMatrix;

		Entity parentEntity = { transform.parent, m_Context.get() };
		if (parentEntity) {
			// Get the inverse matrix of the parent transform world matrix
			Engine::ECS::TransformComponent& parentTransform = parentEntity.GetComponent<Engine::ECS::TransformComponent>();
			glm::mat4 parentTransformMatrix = parentTransform.GetWorldMatrix(m_Context->GetECS());
			parentTransformMatrix = glm::inverse(parentTransformMatrix);
			// Convert the world matrix to a local matrix
			transformLocalMatrix = parentTransformMatrix * transformWorldMatrix;
		}

		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(0.0f);

		ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformLocalMatrix), &position.x, &rotation.x, &scale.x);
		if (m_GizmoType == ImGuizmo::OPERATION::TRANSLATE) transform.position = position;
		if (m_GizmoType == ImGuizmo::OPERATION::SCALE) transform.scale = scale;

		ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(deltaMatrix), &position.x, &rotation.x, &scale.x);

		if (m_GizmoType != ImGuizmo::OPERATION::ROTATE) return;

		// Update rotation
		TransformMetaData& metaData = m_Selected.GetComponent<TransformMetaData>();
		transform.rotation = glm::quat(glm::radians(rotation)) * transform.rotation;
		//metaData.eulerAngles = glm::degrees(glm::eulerAngles(transform.rotation));
		metaData.eulerAngles = glm::eulerAngles(transform.rotation);
	}
	void SceneViewPanel::ShowGizmoOverlayWindow()
	{
		// Configure window
		glm::vec2 windowPosition = m_TopWindowPosition;
		windowPosition += glm::vec2(3.0f, 3.0f);
		ImGui::SetNextWindowPos(ToImVec(windowPosition));
		ImGui::SetNextWindowSize(ImVec2(112, 45));
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGuiWindowFlags overlayFlags = 0;
		overlayFlags |= ImGuiWindowFlags_NoDocking;
		overlayFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
		overlayFlags |= ImGuiWindowFlags_NoMove;
		overlayFlags |= ImGuiWindowFlags_NoResize;
		overlayFlags |= ImGuiWindowFlags_NoTitleBar;

		ImGui::Begin("SceneViewGizmoWindow", nullptr, overlayFlags);

		// Gizmo button helper lamda
		auto GizmoButton = [&](ImGuizmo::OPERATION type, EditorIcon icon, const i8* desc, const i8* shortcut) {
			ImGui::PushID(shortcut);
			ImVec2 cursor = ImGui::GetCursorScreenPos();
			u32 tint = (m_GizmoType == type) ? IM_COL32(255, 255, 255, 255) : IM_COL32(100, 100, 100, 255);
			
			// Create invisible button over the icon
			if (ImGui::InvisibleButton("InvisibleGizmoButton", ImVec2(26, 26))) m_GizmoType = type;
			ImGui::GetWindowDrawList()->AddImage(
				ImGui::ToImGuiTexture(EditorGui::GetIcon(icon)),
				cursor, ImVec2(cursor.x + 30, cursor.y + 30),
				ImVec2(0, 1), ImVec2(1, 0),
				tint
			);

			// Show gizmo description and keyboard shortcut
			if (ImGui::BeginItemTooltip()) {
				ImGui::Text(desc);
				ImGui::SameLine();
				ImGui::TextDisabled(shortcut);
				ImGui::EndTooltip();
			}
			ImGui::PopID();
		};

		// Translation
		GizmoButton(ImGuizmo::OPERATION::TRANSLATE, EditorIcon_Translate, "Translate Gizmo", "W");
		ImGui::SameLine();

		// Scale
		GizmoButton(ImGuizmo::OPERATION::SCALE, EditorIcon_Scale, "Scale Gizmo", "E");
		ImGui::SameLine();

		// Rotate
		GizmoButton(ImGuizmo::OPERATION::ROTATE, EditorIcon_Rotate, "Rotate Gizmo", "R");

		ImGui::End();
	}

	void SceneViewPanel::ShowRuntimeOverlayWindow()
	{
		// Configure window
		glm::vec2 windowSize = { 54, 54 };
		glm::vec2 windowPosition = glm::vec2(0.0f, 0.0f);
		windowPosition.x = ((m_TopWindowPosition.x + m_BottomWindowPosition.x) / 2.0f) - (windowSize.x / 2.0f);
		windowPosition.y = m_TopWindowPosition.y + 3.0f;
		
		ImGui::SetNextWindowPos(ToImVec(windowPosition));
		ImGui::SetNextWindowSize(ToImVec(windowSize));
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGuiWindowFlags overlayFlags = 0;
		overlayFlags |= ImGuiWindowFlags_NoDocking;
		overlayFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
		overlayFlags |= ImGuiWindowFlags_NoMove;
		overlayFlags |= ImGuiWindowFlags_NoResize;
		overlayFlags |= ImGuiWindowFlags_NoTitleBar;

		ImGui::Begin("SceneViewRuntimeControllsWindow", nullptr, overlayFlags);

		// Get the correct icon
		ref<Renderer::Texture> icon = nullptr;
		if      (m_EditorState == EditorState_Editing) icon = EditorGui::GetIcon(EditorIcon_Play);
		else if (m_EditorState == EditorState_Running) icon = EditorGui::GetIcon(EditorIcon_Pause);
		
		// Tint the image if it is hovered
		u32 tint = IM_COL32(255, 255, 255, 255);
		ImVec2 imageMin = ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y);
		ImVec2 imageMax = ImVec2(imageMin.x + 40.0f, imageMin.y + 40.0f);
		ImVec2 mousePos = ImGui::GetMousePos();
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(imageMin, imageMax) && !ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId)) {
			tint = IM_COL32(100, 100, 100, 255);
		}

		// Draw image
		ImGui::GetWindowDrawList()->AddImage(
			ImGui::ToImGuiTexture(icon),
			imageMin, imageMax,
			ImVec2(0.05f, 0.95f), ImVec2(0.95f, 0.05f),
			tint
		);

		// Handle Clicks
		if (ImGui::InvisibleButton("InvisiblePausePlayButton", ImVec2(35, 35))) {
			if (m_EditorState == EditorState_Editing) {
				Event::StartRuntime e;
				Core::Application::EventCallback(e);
			}
			else if (m_EditorState == EditorState_Running) {
				Event::PauseRuntime e;
				Core::Application::EventCallback(e);
			}
		}

		ImGui::End();
	}

	void SceneViewPanel::ShowSettingsOverlayWindow()
	{
		// Configure window
		glm::vec2 windowSize = { 45, 45 };
		glm::vec2 windowPosition = glm::vec2(0.0f, 0.0f);
		windowPosition.x = m_BottomWindowPosition.x - windowSize.x - 3.0f;
		windowPosition.y = m_TopWindowPosition.y + 3.0f;

		ImGui::SetNextWindowPos(ToImVec(windowPosition));
		ImGui::SetNextWindowSize(ToImVec(windowSize));
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGuiWindowFlags overlayFlags = 0;
		overlayFlags |= ImGuiWindowFlags_NoDocking;
		overlayFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
		overlayFlags |= ImGuiWindowFlags_NoMove;
		overlayFlags |= ImGuiWindowFlags_NoResize;
		overlayFlags |= ImGuiWindowFlags_NoTitleBar;

		ImGui::Begin("SceneViewSettingsWindow", nullptr, overlayFlags);

		// Tint the image if it is hovered
		u32 tint = IM_COL32(255, 255, 255, 255);
		ImVec2 imageMin = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
		ImVec2 imageMax = ImVec2(imageMin.x + windowSize.x - 14.0f, imageMin.y + windowSize.y - 14.0f);
		ImVec2 mousePos = ImGui::GetMousePos();
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(imageMin, imageMax) && !ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId)) {
			tint = IM_COL32(100, 100, 100, 255);
		}

		// Draw image
		ImGui::GetWindowDrawList()->AddImage(
			ImGui::ToImGuiTexture(EditorGui::GetIcon(EditorIcon_Menu)),
			imageMin, imageMax,
			ImVec2(0.05f, 0.95f), ImVec2(0.95f, 0.05f),
			tint
		);

		// Handle Clicks
		if (ImGui::InvisibleButton("InvisiblePausePlayButton", ImVec2(windowSize.x - 19.0f, windowSize.y - 19.0f))) {
			ImGui::OpenPopup("SceneViewSettingsMenu");
		}

		ShowSettingsMenu();

		ImGui::End();
	}

	void SceneViewPanel::ShowSettingsMenu()
	{
		if (!ImGui::BeginPopup("SceneViewSettingsMenu")) return;
		ImGui::Checkbox("Show Gizmos", &m_ShowGizmos);
		ImGui::EndPopup();
	}

	Entity SceneViewPanel::GetHoveredEntity()
	{
		// Get mouse position
		i32 mouseX = Engine::Input::GetMouseX();
		i32 mouseY = m_Surface.scale.y - Engine::Input::GetMouseY();

		// Get entity at mouse position
		Engine::ECS::EntityID entityID = 0;
		m_RendererContext->GetFrameBuffer()->GetPixel(mouseX, mouseY, 1, &entityID);
		if (entityID == 0) return {};
		
		return { entityID - 1, m_Context.get() };
	}

	void SceneViewPanel::Render()
	{
		if (m_Context == nullptr) return;

		if (m_EditorState == EditorState_Editing) m_ActiveCamera = m_EditorCamera;
		else if (m_EditorState == EditorState_Running) {
			Engine::ECS::OrthographicCameraComponent& cameraComponent = m_Context->GetMetaData().activeCamera.GetComponent<Engine::ECS::OrthographicCameraComponent>();
			Engine::ECS::TransformComponent cameraTransform = m_Context->GetMetaData().activeCamera.GetComponent<Engine::ECS::TransformComponent>().ApplyParent(m_Context->GetECS());
		
			ref<Renderer::OrthographicCamera> orthoCamera = CastRef<Renderer::OrthographicCamera>(m_RuntimeCamera);
			if (cameraComponent.fitToScreen) cameraComponent.view = Renderer::ViewBox::SurfaceViewBox(m_Surface);
			orthoCamera->SetViewBox(cameraComponent.view);
			orthoCamera->SetZoom(cameraComponent.zoom);
			orthoCamera->SetPosition(cameraTransform.position);

			m_ActiveCamera = orthoCamera;
		}

		m_RendererContext->StartScene(m_Context->GetECS(), m_ActiveCamera);

		using namespace Engine::ECS;

		// Render the RenderComponents
		View<QuadRendererComponent> quadView(m_Context->GetECS());
		quadView.ForEach([&](EntityID id) { m_RendererContext->DrawEntity(id); });

		View<CircleRendererComponent> circleView(m_Context->GetECS());
		circleView.ForEach([&](EntityID id) { m_RendererContext->DrawEntity(id); });

		m_RendererContext->EndScene();
	}
}