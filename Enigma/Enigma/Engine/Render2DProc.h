#pragma once
#include "Core/Process/SubProcess.h"

#include "Renderer/Render2D.h"
#include "Renderer/Camera.h"

namespace Enigma {
	namespace Engine {

		class Render2DProc : public Core::SubProcess {
		public:

			void SetRenderContext(Renderer::Render2D* context) { m_RenderContext = context; }

			virtual void StartUp() override;
			virtual void Render() override;

		private:
			Renderer::Render2D* m_RenderContext;
			Renderer::OrthographicCamera* m_CurrentCamera;
			glm::vec2 m_LastFrameSize;
		};

	}
}