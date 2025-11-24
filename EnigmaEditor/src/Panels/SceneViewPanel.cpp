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
		m_Hovered = false;
		m_Focused = false;
		m_GizmoHovered = false;
		m_CameraSpeed = 7.5f;
		m_ZoomSpeed = 0.125f;
		m_MaxZoom = 1.0f;
		m_MinZoom = 100.0f;
		m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

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
		m_Camera = Renderer::OrthographicCamera::Create(view, 4, { 0.0f,0.0f, view.far - 1.0f });
		
		// Configure render system for when a context is set
		m_RenderSystemConfig.renderAPI = window->GetAPI();
		m_RenderSystemConfig.surface = m_Surface;

		// Configure entity picker shader
		Renderer::ShaderConfig entityPickerShaderConfig;
		entityPickerShaderConfig.vertexPath = "assets/DefaultShaders/EntityPicker.vert";
		entityPickerShaderConfig.pixelPath = "assets/DefaultShaders/EntityPicker.frag";
		m_EntityPickerShader = Renderer::Shader::Create(entityPickerShaderConfig);

		// Configure entity picker buffer
		Renderer::FrameBufferConfig entityPickerBufferConfig;
		entityPickerBufferConfig.width = m_Surface.scale.x;
		entityPickerBufferConfig.height = m_Surface.scale.y;

		Renderer::Attachment entityPickerAttachment;
		entityPickerAttachment.type           = Renderer::AttachmentType::ColorAttachment;
		entityPickerAttachment.internalFormat = Renderer::TexFormat::RED32I;
		entityPickerAttachment.format         = Renderer::TexFormat::RED_INT;

		entityPickerBufferConfig.attachments = { entityPickerAttachment };
		m_EntityPickerBuffer = Renderer::FrameBuffer::Create(entityPickerBufferConfig);
	}
	void SceneViewPanel::SetContext(ref<Scene> context)
	{
		m_Context = context;

		if (context == nullptr) {
			m_RenderSystem = nullptr;
			return;
		}

		m_RenderSystem = Engine::ECS::RenderSystem2D::Create(m_RenderSystemConfig, m_Context->GetECS());
		m_RenderSystem->Resize(m_Surface.scale.x, m_Surface.scale.y);
	}
	void SceneViewPanel::SetSelected(Entity entity)
	{
		m_Selected = entity;
	}

	void SceneViewPanel::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);
		handler.Dispatch<Core::Keyboard>([&](Core::Keyboard& e) { OnKeyboard(e); return false; });
		handler.Dispatch<Core::MouseButton>([&](Core::MouseButton& e) { OnMouse(e); return false; });
		handler.Dispatch<Core::MouseScroll>([&](Core::MouseScroll& e) { OnScroll(e); return false; });
		
		handler.Dispatch<EntitySelected>([&](EntitySelected& e) {
			m_Selected = e.GetEntity();
			return false;
		});
		handler.Dispatch<SceneChange>([&](SceneChange& e) {
			m_Selected = {};
			SetContext(e.GetScene());
			return false;
		});
	}
	void SceneViewPanel::OnMouse(Core::MouseButton& e)
	{
		if (!m_Hovered) return;
		if (m_GizmoHovered) return;
		if (e.GetButton() != Engine::KeyCode::MouseButtonLeft) return;

		// Get mouse position
		i32 mouseX = Engine::Input::GetMouseX();
		i32 mouseY = m_Surface.scale.y - Engine::Input::GetMouseY();

		// Get entity at mouse position
		Engine::ECS::EntityID entityID = 0;
		m_EntityPickerBuffer->GetPixel(mouseX, mouseY, 0, &entityID);
		if (entityID == 0) return;
		entityID -= 1;

		// Create selection evet
		m_Selected = { entityID, m_Context.get() };
		EntitySelected selectionEvent(m_Selected);
		Core::Application::EventCallback(selectionEvent);
	}
	void SceneViewPanel::OnScroll(Core::MouseScroll& e)
	{
		if (!Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight)) return;
		if (!m_Hovered) return;

		// Get camera zoom
		ref<Renderer::OrthographicCamera> orthoCamera = CastRef<Renderer::OrthographicCamera>(m_Camera);
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
			f32 zoom = CastRef<Renderer::OrthographicCamera>(m_Camera)->GetZoom();
			f32 speed = m_CameraSpeed * (zoom / 4.0f);
			cameraPos += glm::vec3(moveDir * speed * (float)deltaTime, 0.0f);
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
			m_EntityPickerBuffer->Resize(region.x, region.y);
			m_Camera->Resize(region.x, region.y);
		}

		ImVec2 overlayPosition = ImGui::GetCursorScreenPos();
		overlayPosition.x += 3;
		overlayPosition.y += 3;

		// Show texture
		ImGui::Image(m_Frame);
		//ImGui::Image(m_EntityPickerBuffer->GetColorAttachment(0));

		// Process window status
		m_Hovered = ImGui::IsItemHovered();
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && m_Hovered) {
			ImGui::SetWindowFocus();
		}
		m_Focused = ImGui::IsWindowFocused();

		// Gizmos
		if (m_Selected && m_GizmoType != -1) ShowGizmos();
		else m_GizmoHovered = false;

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
		m_GizmoHovered = ImGuizmo::IsOver();
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
		if (m_Context == nullptr) return;

		EntityPicker();

		m_RenderSystem->StartFrame(m_Camera);

		m_RenderSystem->EndFrame();
	}

	void SceneViewPanel::EntityPicker()
	{
		using namespace Engine::ECS;

		Core::Application::UseRenderAPI(m_RenderSystem->GetConfig().renderAPI);
		Renderer::RenderAPI::Clear();

		m_EntityPickerBuffer->Bind();
		Renderer::RenderAPI::Clear();

		m_EntityPickerShader->Bind();

		View<Transform, ColoredQuad> coloredQuadView(m_Context->GetECS());

		glm::mat4 viewProjection = m_Camera->GetViewProjection();
		m_EntityPickerShader->SetUniform("ViewProjection", &viewProjection);

		RenderSystem2D::GetQuad()->Bind();
		coloredQuadView.ForEach([&](EntityID entity, Transform& transform, ColoredQuad& quad) {
			int id = entity;
			m_EntityPickerShader->SetUniform("EntityID", &id);

			glm::mat4 model = transform.GetWorldMatrix(m_Context->GetECS());
			m_EntityPickerShader->SetUniform("Model", (void*)&model);

			Renderer::RenderAPI::DrawIndexed(6, Renderer::DataType::UnsignedInt, NULL);
		});
		RenderSystem2D::GetQuad()->Unbind();

		m_EntityPickerShader->Unbind();

		m_EntityPickerBuffer->Unbind();
	}
}