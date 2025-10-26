#pragma once
#include <Enigma/Core/Process/SubProcess.h>

namespace Enigma::Editor {
	class EditorProcess : public Core::SubProcess {
	public:
		virtual void StartUp() override;
		virtual void ShutDown() override;

		virtual bool OnEvent(Core::Event& e) override;

		virtual void Update(Engine::DeltaTime deltaTime) override;
		virtual void Render() override;
		virtual void ImGui() override;

	private:
		Core::ID m_WindowID;
	};	
}
