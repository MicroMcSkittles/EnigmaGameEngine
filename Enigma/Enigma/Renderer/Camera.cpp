#include "Renderer/Camera.h"
#include "Core/Window.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Enigma {
	namespace Renderer {
		glm::vec2 Camera::WorldToScreen(const glm::vec3& worldPoint) {
			/*glm::vec2 screenPos = m_ViewProjection * glm::vec4(worldPoint, 1.0f);
			auto window = Core::Window::Get();
			screenPos.x = (float)window->GetWidth() * ((screenPos.x + 1.0f) / 2.0f);
			screenPos.y = (float)window->GetHeight() - ((float)window->GetHeight() * ((screenPos.y + 1.0f) / 2.0f));
			return screenPos;*/
			return { 0,0 };
		}
		glm::vec3 Camera::ScreenToWorld(const glm::vec2& screenPoint, const Engine::Surface& surface) {
			glm::vec2 projView;
			projView.x = (screenPoint.x * 2) / surface.scale.x - 1.0f;
			projView.y = ((surface.scale.y - screenPoint.y) * 2) / surface.scale.y - 1.0f;
			return glm::inverse(m_ViewProjection) * glm::vec4(projView, 0.0f, 1.0f);
			return { 0,0,0 };
		}

		Frustum Frustum::SurfaceFrustum(Engine::Surface surface)
		{
			Frustum frustum;
			frustum.fov = glm::radians(45.0f);
			frustum.near = 0.1f;
			frustum.far = 1000.0f;

			frustum.aspectRatio = surface.scale.x / surface.scale.y;
			return frustum;
		}

		PerspectiveCamera::PerspectiveCamera(const Frustum& frustum, const glm::vec3& position, const glm::vec3& direction)
			: Camera(position, direction), m_Frustum(frustum)
		{
			m_Projection = glm::perspective(m_Frustum.fov, m_Frustum.aspectRatio, m_Frustum.near, m_Frustum.far);
			m_InvProjection = glm::inverse(m_Projection);
			CalculateView();
		}
		void PerspectiveCamera::Resize(uint32_t width, uint32_t height)
		{
			m_Frustum.aspectRatio = (float)width / (float)height;
			CalculateProjection();
		}
		void PerspectiveCamera::CalculateView()
		{
			m_View = glm::lookAt(m_Position, m_Position + m_Direction, glm::vec3(0.0f, 1.0f, 0.0f));
			m_InvView = glm::inverse(m_View);
			m_ViewProjection = m_Projection * m_View;
		}
		void PerspectiveCamera::CalculateProjection()
		{
			m_Projection = glm::perspective(m_Frustum.fov, m_Frustum.aspectRatio, m_Frustum.near, m_Frustum.far);
			m_InvProjection = glm::inverse(m_Projection);
			m_ViewProjection = m_Projection * m_View;
		}

		ViewBox ViewBox::Zoom(float zoom) const
		{
			ViewBox view;
			view.top = top * zoom;
			view.bottom = bottom * zoom;
			view.left = left * zoom;
			view.right = right * zoom;

			view.near = near * zoom;
			view.far = far * zoom;
			return view;
		}

		ViewBox ViewBox::SurfaceViewBox(Engine::Surface surface)
		{
			ViewBox view;

			float aspectRatio = (float)surface.scale.x / (float)surface.scale.y;

			view.left = -aspectRatio;
			view.right = aspectRatio;

			view.top = 1;
			view.bottom = -1;
			view.near = 0.1f;
			view.far = 1000.0f;

			return view;
		}

		OrthographicCamera::OrthographicCamera(const ViewBox& viewBox, float zoom, const glm::vec3& position, const glm::vec3& direction)
			:Camera(position, direction)
		{
			m_Zoom = zoom;
			m_ViewBox = viewBox;
			m_ZoomViewBox = viewBox.Zoom(m_Zoom);

			m_Projection = glm::ortho(m_ZoomViewBox.left, m_ZoomViewBox.right, m_ZoomViewBox.bottom, m_ZoomViewBox.top, m_ZoomViewBox.near, m_ZoomViewBox.far);
			CalculateView();
		}

		void OrthographicCamera::Resize(uint32_t width, uint32_t height)
		{
			float aspectRatio = (float)width / (float)height;
			m_ViewBox.left = -aspectRatio;
			m_ViewBox.right = aspectRatio;
			m_ZoomViewBox = m_ViewBox.Zoom(m_Zoom);
			CalculateProjection();
		}

		void OrthographicCamera::CalculateView()
		{
			m_View = glm::lookAt(m_Position, m_Position + m_Direction, glm::vec3(0.0f, 1.0f, 0.0f));
			m_ViewProjection = m_Projection * m_View;
		}

		void OrthographicCamera::CalculateProjection()
		{
			m_Projection = glm::ortho(m_ZoomViewBox.left, m_ZoomViewBox.right, m_ZoomViewBox.bottom, m_ZoomViewBox.top, m_ZoomViewBox.near, m_ZoomViewBox.far);
			m_ViewProjection = m_Projection * m_View;
		}
		void OrthographicCamera::SetViewBox(const ViewBox& view)
		{
			m_ViewBox = view;
			m_ZoomViewBox = m_ViewBox.Zoom(m_Zoom);
			CalculateProjection();
		}
		void OrthographicCamera::SetZoom(float zoom)
		{
			m_Zoom = zoom;
			m_ZoomViewBox = m_ViewBox.Zoom(m_Zoom);
			CalculateProjection();
		}
		
}
}