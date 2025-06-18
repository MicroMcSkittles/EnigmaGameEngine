#pragma once
#include "Core/Process/SubProcess.h"
#include "Core/Event/WindowEvent.h"
#include "Renderer/Camera.h"

#include "ECS/Entity.h"

namespace Enigma {
	namespace Core {

		// TODO: fix this, it sucks
		class RenderProc : public SubProcess {
		public:

			virtual void StartUp() override;
			virtual void ShutDown() override;

			bool OnResize(Core::WindowResize& e);
			virtual bool OnEvent(Event& e) override;
			virtual void Update(float deltaTime) override;
			virtual void Render() override;

		private:
			Renderer::Camera* m_ActiveCamera;
			ECS::Entity* m_ActiveCameraEntity;
			int m_SearchCounter; // used to search for a camera entity every couple frames
		};

	}
}