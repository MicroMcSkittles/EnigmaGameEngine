#pragma once
#include "Enigma/Engine/Surface.h"
#include <glm/glm.hpp>

namespace Enigma {
	namespace Renderer {
		class Camera {
		public:
			Camera(const glm::vec3& position, const glm::vec3& direction)
				: m_Position(position), m_Direction(direction) { }

			virtual void Resize(u32 width, u32 height) = 0;

			virtual void CalculateView() = 0;
			virtual void CalculateProjection() = 0;

			const glm::vec3& GetPosition() const { return m_Position; }
			const glm::vec3& GetDirection() const { return m_Direction; }

			void SetPosition(const glm::vec3& position) { m_Position = position; CalculateView(); }
			void SetDirection(const glm::vec3& direction) { m_Direction = direction; CalculateView(); }

			const glm::mat4& GetView() const { return m_View; }
			const glm::mat4& GetInvView() const { return m_InvView; }
			const glm::mat4& GetProjection() const { return m_Projection; }
			const glm::mat4& GetInvProjection() const { return m_InvProjection; }
			const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

			glm::vec2 WorldToScreen(const glm::vec3& worldPoint);
			glm::vec3 ScreenToWorld(const glm::vec2& screenPoint, const Engine::Surface& surface);

		protected:
			glm::vec3 m_Position;
			glm::vec3 m_Direction;

			glm::mat4 m_View;
			glm::mat4 m_InvView;
			glm::mat4 m_Projection;
			glm::mat4 m_InvProjection;
			glm::mat4 m_ViewProjection;
		};

		struct Frustum {
			f32 fov;
			f32 aspectRatio;
			f32 near;
			f32 far;

			static Frustum SurfaceFrustum(Engine::Surface surface);
		};

		class PerspectiveCamera : public Camera {
		public:
			static ref<PerspectiveCamera> Create(const Frustum& frustum, const glm::vec3& position, const glm::vec3& direction);
			PerspectiveCamera(const Frustum& frustum, const glm::vec3& position, const glm::vec3& direction);

			virtual void Resize(u32 width, u32 height) override;

			virtual void CalculateView() override;
			virtual void CalculateProjection() override;

			const Frustum& GetFrustum() { return m_Frustum; }

		private:
			Frustum m_Frustum;
		};

		struct ViewBox {
			f32 top;
			f32 bottom;
			f32 left;
			f32 right;
			f32 near;
			f32 far;

			ViewBox Zoom(f32 zoom) const;

			static ViewBox SurfaceViewBox(Engine::Surface surface);
		};

		class OrthographicCamera : public Camera {
		public:
			static ref<OrthographicCamera> Create(const ViewBox& viewBox, f32 zoom = 1.0f, const glm::vec3& position = { 0,0,0 }, const glm::vec3& direction = { 0,0,-1 });
			OrthographicCamera(const ViewBox& viewBox, f32 zoom, const glm::vec3& position, const glm::vec3& direction);

			virtual void Resize(u32 width, u32 height) override;

			virtual void CalculateView() override;
			virtual void CalculateProjection() override;

			const ViewBox& GetViewBox() { return m_ViewBox; }
			const ViewBox& GetZoomViewBox() { return m_ZoomViewBox; }
			void SetViewBox(const ViewBox& view);

			f32 GetZoom() { return m_Zoom; }
			void SetZoom(f32 zoom);

		private:
			ViewBox m_ViewBox;
			ViewBox m_ZoomViewBox;
			f32 m_Zoom;
		};
	}
}