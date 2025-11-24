#include "Panels/SceneViewPanel.h"
#include "Scene/Components.h"
#include "EditorImGui.h"
#include "EditorEvents.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>

#include <Enigma/Engine/Input.h>
#include <Enigma/Engine/InputCodes.h>

#include <imgui.h>
#include <Enigma/ImGui/EnigmaWidgets.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

namespace Enigma::Editor {
	SceneViewPanel::SceneViewPanel(Core::ID windowID) : m_WindowID(windowID)
	{
		m_CameraSpeed = 5.0f;
		m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}
	void SceneViewPanel::SetContext(ref<Scene> context)
	{
		m_Context = context;
		
		ref<Core::Window> window = Core::Application::GetWindow(m_WindowID);

		Core::Application::UseRenderAPI(window->GetAPI());

		Renderer::TextureConfig frameConfig;
		frameConfig.width = window->GetWidth();
		frameConfig.height = window->GetHeight();
		m_Frame = Renderer::Texture::Create(frameConfig);

		m_Surface.position = { 0.0f, 0.0f };
		m_Surface.scale = { window->GetWidth(), window->GetHeight() };
		m_Surface.frame = m_Frame;

		Engine::InputConfig inputConfig;
		inputConfig.surface = &m_Surface;
		inputConfig.window = window;
		m_InputContext = Engine::Input::Create(inputConfig);

		Engine::ECS::RenderSystem2DConfig renderConfig;
		renderConfig.renderAPI = window->GetAPI();
		renderConfig.surface = m_Surface;

		Renderer::ViewBox view = Renderer::ViewBox::SurfaceViewBox(renderConfig.surface);
		m_Camera = Renderer::OrthographicCamera::Create(view, 4, { 0.0f,0.0f,100.0f });

		m_RenderSystem = Engine::ECS::RenderSystem2D::Create(renderConfig, m_Context->GetECS());
	}
	void SceneViewPanel::SetSelected(Entity entity)
	{
		m_Selected = entity;
	}

	void SceneViewPanel::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);
		handler.Dispatch<Core::Keyboard>([&](Core::Keyboard& e) { OnKeyboard(e); return false; });
		
		handler.Dispatch<EntitySelected>([&](EntitySelected& e) {
			m_Selected = e.GetEntity();
			return false;
		});
		handler.Dispatch<SceneChange>([&](SceneChange& e) {
			SetContext(e.GetScene());
			return false;
		});
	}
	void SceneViewPanel::OnKeyboard(Core::Keyboard& e)
	{
		if (e.GetAction() != Engine::KeyCode::KeyPress) return;

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
		if (!m_Hovered || !Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight)) return;

		// Get movement direction
		glm::vec2 moveDir = glm::vec2(0.0f);
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyW)) moveDir.y += 1.0f;
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyA)) moveDir.x += -1.0f;
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyS)) moveDir.y += -1.0f;
		if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyD)) moveDir.x += 1.0f;

		// Update camera position
		if (moveDir.x || moveDir.y) {
			moveDir = glm::normalize(moveDir);
			glm::vec3 cameraPos = m_Camera->GetPosition();
			cameraPos += glm::vec3(moveDir * m_CameraSpeed * (float)deltaTime, 0.0f);
			m_Camera->SetPosition(cameraPos);
		}
	}

	void SceneViewPanel::ShowGui()
	{
		ImGui::Begin("Scene View");

		Engine::Input::MakeCurrent(m_InputContext);
		ImGui::Text("Mouse Pos ( %.1f, %.1f ), Selected Entity: %u", Engine::Input::GetMouseX(), Engine::Input::GetMouseY(), m_Selected.GetID());

		// Get the size of ImGui window
		glm::vec2 region = FromImVec(ImGui::GetContentRegionAvail());
		m_Surface.position = FromImVec(ImGui::GetCursorScreenPos());

		// Make sure the texture is the right size
		if (m_Surface.scale != region) {
			m_Surface.Resize(region.x, region.y);
			m_RenderSystem->Resize(region.x, region.y);
			m_Camera->Resize(region.x, region.y);
		}

		ImVec2 overlayPosition = ImGui::GetCursorScreenPos();
		overlayPosition.x += 3;
		overlayPosition.y += 3;

		// Show texture
		ImGui::Image(m_Frame);
		m_Hovered = ImGui::IsItemHovered();

		// Gizmos
		if (m_Selected && m_GizmoType != -1) ShowGizmos();

		// Show overlay window
		ShowOverlayWindow(overlayPosition.x, overlayPosition.y);

		ImGui::End();
	}
	void SceneViewPanel::ShowGizmos()
	{
		// Configure ImGuizmo
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_Surface.position.x, m_Surface.position.y, m_Surface.scale.x, m_Surface.scale.y);

		// Camera
		const glm::mat4& cameraProj = m_Camera->GetProjection();
		const glm::mat4& cameraView = m_Camera->GetView();

		// Entity transform
		Engine::ECS::Transform& transform = m_Selected.GetComponent<Engine::ECS::Transform>();
		glm::mat4 transformWorldMatrix = transform.GetWorldMatrix(m_Context->GetECS());

		ImGuizmo::Manipulate(
			glm::value_ptr(cameraView),
			glm::value_ptr(cameraProj),
			static_cast<ImGuizmo::OPERATION>(m_GizmoType),
			ImGuizmo::MODE::LOCAL,
			glm::value_ptr(transformWorldMatrix)
		);

		// Process input
		if (!ImGuizmo::IsUsing()) return;

		glm::mat4 transformLocalMatrix = transformWorldMatrix;

		Entity parentEntity = { transform.parent, m_Context.get() };
		if (parentEntity) {
			// Get the inverse matrix of the parent transform world matrix
			Engine::ECS::Transform& parentTransform = parentEntity.GetComponent<Engine::ECS::Transform>();
			glm::mat4 parentTransformMatrix = parentTransform.GetWorldMatrix(m_Context->GetECS());
			parentTransformMatrix = glm::inverse(parentTransformMatrix);
			// Convert the world matrix to a local matrix
			transformLocalMatrix = parentTransformMatrix * transformWorldMatrix;
		}

		glm::vec3 rotation = glm::vec3(0.0f);
		ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformLocalMatrix), &transform.position.x, &rotation.x, &transform.scale.x);

		// Update rotation
		EntityMetaData& metaData = m_Selected.GetComponent<EntityMetaData>();
		metaData.eulerAngles = rotation;
		transform.rotation = glm::rotate(glm::quat(), glm::radians(metaData.eulerAngles));
	}
	void SceneViewPanel::ShowOverlayWindow(f32 x, f32 y)
	{
		// Configure window
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowSize(ImVec2(57, 35));
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGuiWindowFlags overlayFlags = 0;
		overlayFlags |= ImGuiWindowFlags_NoDocking;
		overlayFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
		overlayFlags |= ImGuiWindowFlags_NoMove;
		overlayFlags |= ImGuiWindowFlags_NoResize;
		overlayFlags |= ImGuiWindowFlags_NoTitleBar;

		ImGui::Begin("SceneViewGizmoWindow", nullptr, overlayFlags);

		// Gizmo button helper lamda
		auto GizmoButton = [&](ImGuizmo::OPERATION type, const i8* name, const i8* desc, const i8* shortcut) {
			ImGui::PushID(name);
			if (m_GizmoType == type) ImGui::Text(name);
			else {
				// Create invisible button over the text
				ImVec2 cursor = ImGui::GetCursorPos();
				if (ImGui::InvisibleButton("InvisibleGizmoButton", ImGui::CalcTextSize(name))) {
					m_GizmoType = type;
				}
				ImGui::SetCursorPos(cursor);
				ImGui::TextDisabled(name);
			}
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
		GizmoButton(ImGuizmo::OPERATION::TRANSLATE, "T", "Transform Gizmo", "W");
		ImGui::SameLine();

		// Scale
		GizmoButton(ImGuizmo::OPERATION::SCALE, "S", "Scale Gizmo", "E");
		ImGui::SameLine();

		// Rotate
		GizmoButton(ImGuizmo::OPERATION::ROTATE, "R", "Rotate Gizmo", "R");

		ImGui::End();
	}
	
	void SceneViewPanel::Render()
	{
		m_RenderSystem->StartFrame(m_Camera);

		m_RenderSystem->EndFrame();
	}
}