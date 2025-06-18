#pragma once
#include "Core/Process/SubProcess.h"
#include "Renderer/Camera.h"

namespace Enigma {
	namespace Core {

		class RenderProc : public SubProcess {
		public:

			virtual void StartUp() override;
			virtual void ShutDown() override;

			virtual bool OnEvent(Event& e) override;
			virtual void Update(float deltaTime) override;
			virtual void Render() override;

		private:
			Renderer::Camera* m_ActiveCamera;
			int m_SearchCounter; // used to search for a camera entity every couple frames
		};

	}
}