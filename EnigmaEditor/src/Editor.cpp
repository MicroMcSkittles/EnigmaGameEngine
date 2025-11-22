#include "Editor.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>

#include <imgui.h>
#include <ImGuizmo.h>

#include "EditorImGui.h"
#include <Enigma/Engine/ECS/Components.h>

//#include <EnigmaSerialization/YAML.h>
#include "Serialization/SceneSerializer.h"

//static int GUARD_BAND_SIZE_BYTES = 64;
//
//static void* MyCustomAlloc(size_t userNumBytes)
//{
//	// We'll allocate space for a guard-band, then space to store the user's allocation-size-value,
//	// then space for the user's actual data bytes, then finally space for a second guard-band at the end.
//	char* buf = (char*)malloc(GUARD_BAND_SIZE_BYTES + sizeof(userNumBytes) + userNumBytes + GUARD_BAND_SIZE_BYTES);
//	if (buf)
//	{
//		char* w = buf;
//		memset(w, 'B', GUARD_BAND_SIZE_BYTES);          w += GUARD_BAND_SIZE_BYTES;
//		memcpy(w, &userNumBytes, sizeof(userNumBytes)); w += sizeof(userNumBytes);
//		char* userRetVal = w;                          w += userNumBytes;
//		memset(w, 'E', GUARD_BAND_SIZE_BYTES);          w += GUARD_BAND_SIZE_BYTES;
//		printf("Allocated %u: %p\n", userNumBytes, userRetVal);
//		return userRetVal;
//	}
//	else throw std::bad_alloc();
//}
//
//static void MyCustomDelete(void* p)
//{
//	if (p == NULL) return;   // since delete NULL is a safe no-op
//
//	// Convert the user's pointer back to a pointer to the top of our header bytes
//	char* internalCP = ((char*)p) - (GUARD_BAND_SIZE_BYTES + sizeof(size_t));
//
//	char* cp = internalCP;
//	for (int i = 0; i < GUARD_BAND_SIZE_BYTES; i++)
//	{
//		if (*cp++ != 'B')
//		{
//			printf("CORRUPTION DETECTED at BEGIN GUARD BAND POSITION %i of allocation %p\n", i, p);
//			abort();
//		}
//	}
//
//	// At this point, (cp) should be pointing to the stored (userNumBytes) field
//	size_t userNumBytes = *((const size_t*)cp);
//	cp += sizeof(userNumBytes);  // skip past the user's data
//	cp += userNumBytes;
//
//	// At this point, (cp) should be pointing to the second guard band
//	for (int i = 0; i < GUARD_BAND_SIZE_BYTES; i++)
//	{
//		if (*cp++ != 'E')
//		{
//			printf("CORRUPTION DETECTED at END GUARD BAND POSITION %i of allocation %p\n", i, p);
//			abort();
//		}
//	}
//
//	// If we got here, no corruption was detected, so free the memory and carry on
//	printf("Dealocated %u: %p\n", userNumBytes, internalCP);
//	free(internalCP);
//}
//
//// override the global C++ new/delete operators to call our
//// instrumented functions rather than their normal behavior
//void* operator new(size_t s)    throw(std::bad_alloc) { return MyCustomAlloc(s); }
//void* operator new[](size_t s)  throw(std::bad_alloc) {return MyCustomAlloc(s); }
//void operator delete(void* p)   throw() { MyCustomDelete(p); }
//void operator delete[](void* p) throw() {MyCustomDelete(p); }

namespace Enigma::Editor {
	void EditorProcess::StartUp()
	{

		// Create Window
		Core::WindowConfig windowConfig;
		windowConfig.title     = "Enigma Editor";
		windowConfig.width     = 1024;
		windowConfig.height    = 720;
		windowConfig.renderAPI = Renderer::API::OpenGL;

		Core::ImGuiConfig imguiConfig;
		imguiConfig.renderAPI = windowConfig.renderAPI;

		m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);
		Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) { return OnEvent(e); });
		Core::Application::BindSubProcToWindow(Core::Application::GetSubProcess(m_ProcessID), m_WindowID);

		// Init RenderAPI
		Core::Application::UseRenderAPI(windowConfig.renderAPI);
		Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit);
		Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });

		// Init Gui
		EditorStyle style;
		EditorGui::SetStyle(style);

		// Setup scene
		/*m_Scene = Scene::Create();
		m_Entity = m_Scene->CreateEntity("Cube");
		m_Scene->CreateEntity(m_Entity, "Child 1");
		m_Scene->CreateEntity(m_Entity, "Child 2");
		m_Scene->CreateEntity("Box even");
		m_Scene->CreateEntity("Circle");

		m_Entity.CreateComponent<Engine::ECS::ColoredQuad>(glm::vec3(1.0f, 0.0f, 0.0f));*/

		{
			ref<Scene> scene = Scene::Create();
			Entity entity = scene->CreateEntity("Cube");
			scene->CreateEntity(entity, "Child 1");
			scene->CreateEntity(entity, "Child 2");
			scene->CreateEntity("Box even");
			scene->CreateEntity("Circle");

			entity.CreateComponent<Engine::ECS::ColoredQuad>(glm::vec3(1.0f, 0.0f, 0.0f));

			SceneSerializer serializer(scene);
			serializer.Serialize("Scene.yml");
		}

		m_Scene = Scene::Create();
		SceneSerializer serializer(m_Scene);
		serializer.Deserialize("Scene.yml");

		// Configure Inspector
		m_InspectorPanel = CreateUnique<InspectorPanel>();

		// Configure Scene View
		m_SceneViewPanel = CreateUnique<SceneViewPanel>(m_WindowID);
		m_SceneViewPanel->SetContext(m_Scene);

		// Configure Scene Hierachy
		m_SceneHierachyPanel = CreateUnique<SceneHierachyPanel>();
		m_SceneHierachyPanel->SetContext(m_Scene);
		m_SceneHierachyPanel->SetSelectionCallback([&](Entity selected) {
			m_SceneViewPanel->SetSelected(selected);
			m_InspectorPanel->SetContext(EntityInspectorContext::Create(selected));
		});

	}
	void EditorProcess::ShutDown()
	{
	}

	bool EditorProcess::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);

		// Stop the application when the window closes
		handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& windowEvent) {
			Core::Application::Close();
			return false;
		});

		m_SceneViewPanel->OnEvent(e);

		return false;
	}

	void EditorProcess::Update(Engine::DeltaTime deltaTime)
	{
		m_SceneViewPanel->Update(deltaTime);
	}
	void EditorProcess::Render()
	{
		Renderer::RenderAPI::Clear();
		m_SceneViewPanel->Render();
	}

	void EditorProcess::ImGui()
	{
		ImGuizmo::BeginFrame();

		m_InspectorPanel->ShowGui();
		m_SceneHierachyPanel->ShowGui();
		m_SceneViewPanel->ShowGui();
	}
}