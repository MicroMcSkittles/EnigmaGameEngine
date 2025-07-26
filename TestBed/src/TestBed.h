#pragma once
#include <Enigma/Core/Process/SubProcess.h>



using namespace Enigma;

class TestContext {
public:
	TestContext(Core::ID windowID) : m_WindowID(windowID) { }

	virtual void OnEvent(Core::Event& e) { }
	virtual void Update(Engine::DeltaTime deltaTime) { }
	virtual void Render() { }
	virtual void ImGui() { }

protected:
	Core::ID m_WindowID;
};

class TestBed : public Core::SubProcess {
public:
	virtual void StartUp() override;

	void OnClose();
	virtual bool OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime);
	virtual void Render() override;
	virtual void ImGui() override;

private:
	Core::ID m_WindowID;
	TestContext* m_TestContext;
};
