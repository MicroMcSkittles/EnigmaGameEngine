 #pragma once
 #include "Enigma/Core/Process/SubProcess.h"

 #include "Enigma/Renderer/Renderer2D/Render2D.h"
 #include "Enigma/Renderer/Camera.h"

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