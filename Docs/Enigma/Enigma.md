# Getting Started
There are a few basic systems you must now to get started with Enigma. The following should explain the basics.
This is still a work in progress, some pages may not be done yet. It'll be updated periodically.
## Entry Point
The [entry point](Core/EntryPoint.md) is handeled by Enigma, so you must define the `ApplicationMain` function.
```C++
#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
	
}
```
The `Enigma/Enigma.h` file includes necessary core headers.
The `Enigma/Core/EntryPoint.h` file should only be included in one `.cpp` file. It contains the main function.
`ApplicationMain` gets called before the [Application](Core/Process/Application.md) gets run.

## Sub Processes
A [Sub Process](Core/Process/SubProcess.md) is a user defined process that both the [Application](Core/Process/Application.md) and [Engine Instances](Engine/Engine.md) use to let the user add more functionality.
The following is a example of a basic sub process.
```C++
class MySubProcess : public Core::SubProcess {
public:
    // Runs the frame after MySubProcess gets created
    virtual void StartUp() override {
        LOG_MESSAGE("Started my sub process", 5);
    }

    // Runs when MySubProcess gets destroyed
    virtual void ShutDown() override {
        LOG_MESSAGE("Shutting down my sub process", 5);
    }
};
```
You can create a [Sub Process](Core/Process/SubProcess.md/#sub-process) with the `CreateSubProc` function inside the [Application](Core/Process/Application.md) and [Engine](Engine/Engine.md) classes. This function is a template function that returns T*.
```C++
void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
    MySubProcess* process = app->CreateSubProc<MySubProcess>();
}
```
Some functionality of a [Sub Process](Core/Process/SubProcess.md/#sub-process) only works if bound to either a [Window](Core/Window.md) or Engine Instance. The [Sub Process](Core/Process/SubProcess.md/#sub-process) is automaticaly bound to a Engine Instance if it is created with that Engine Instances `CreateSubProc` function.
To bind a [Sub Process](Core/Process/SubProcess.md/#sub-process) to a Window use the function `BindSubProcToWindow` found in the Application class. This function takes in a pointer to the [Sub Process](Core/Process/SubProcess.md/#sub-process) and the ID of the Window you want to bind to.
```C++
class MySubProcess : public Core::SubProcess {
public:
    virtual void StartUp() override {
        // Bind MySubProcess to m_WindowID
        Core::Application::BindSubProcToWindow(this, m_WindowID);
    }

private:
    Core::ID m_WindowID;
};
```
## Windows
Many systems require a [Window](Core/Window.md) to work properly. To create a [Window](Core/Window.md) you must use the function `CreateWindow` in the [Application](Core/Process/Application.md) class. This function returns a [ID](Core/IdHandler.md/#id-struct) that refers to the [Window](Core/Window.md). It has 2 overriders, one creates a normal [Window](Core/Window.md), the other creates a [Window](Core/Window.md) with [ImGui](Core/ImGui.md). Both the [Window](Core/Window.md) and ImGui have configuration structs called `WindowConfig` and `ImGuiConfig` respectively.
The following is an example of how to create a [Window](Core/Window.md) with [ImGui](Core/ImGui.md).
```C++
Core::ID CreateMainWindow() {
    // Configure Window
    Core::WindowConfig windowConfig;
    // Window dimensions
    windowConfig.width = 800;
    windowConfig.height = 600;
    // Tell the Window to use OpenGL
    windowConfig.renderAPI = Renderer::API::OpenGL;
    // Set title
    windowConfig.title = "My Window";

    // Configure ImGui
    Core::ImGuiConfig imguiConfig;
    imguiConfig.docking = false;

    // Create window
    Core::ID windowID = Core::Application::CreateWindow(windowConfig, imguiConfig);

    return windowID;
}
```
## Engine Instances
The [Engine Instances](Engine/Engine.md) holds most of the functionality of Enigma.
To create a [Engine Instances](Engine/Engine.md) you must use the function `CreateEngineInstance` in the [Application](Core/Process/Application.md) class. This function returns a [ID](Core/IdHandler.md/#id-struct) that refers to the [Engine Instances](Engine/Engine.md). It takes in a engine configuration struct called `EngineConfig`.
The following is an example of how to create a [Engine Instances](Engine/Engine.md).
```C++
Core::ID CreateMainEngine(Core::ID windowID) {
    // Configure Renderer
    Renderer::Render2DConfig renderConfig;
    renderConfig.renderAPI = Renderer::API::OpenGL;
    // Configure Renderer's Frame
    renderConfig.surface.frame = nullptr;

    // Create & Configure Renderer's Shaders
    Renderer::ShaderConfig shaderConfig;
    // Main Shader
    shaderConfig.vertexPath = "ExampleApp/assets/mainShader.vert";
    shaderConfig.pixelPath = "ExampleApp/assets/mainShader.frag";
    renderConfig.mainShader = Renderer::Shader::Create(shaderConfig);
    // Post Process Shader
    shaderConfig.vertexPath = "ExampleApp/assets/postProcShader.vert";
    shaderConfig.pixelPath = "ExampleApp/assets/postProcShader.frag";
    renderConfig.postProcShader = Renderer::Shader::Create(shaderConfig);

    // Configure Engine Instance
    Engine::EngineConfig engineConfig;
    engineConfig.windowID = windowID;
    engineConfig.rendererConfig = &renderConfig;

    Core::ID instanceID = Core::Application::CreateEngineInstance(engineConfig);
    return instanceID;
}
```
I'm sure you noticed how 90% of this function is configuring the [Renderer](Renderer/Renderer.md). If you'd like to go more in depth into this, then goto the [Renderer](Renderer/Renderer.md) file.
As mentioned in the [Sub Process](Core/Process/SubProcess.md/#sub-process) portion, you can bind [Sub Processes](Core/Process/SubProcess.md/#sub-process) to [Engine Instances](Engine/Engine.md) using the `CreateSubProc` function.
```C++
class EngineSubProc : public Core::SubProcess {
public:
    virtual void StartUp() override {
        LOG_MESSAGE("Started Engine Sub Process!", 5);
    }
}

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
    Core::ID windowID = CreateMainWindow();
    Core::ID engineInstanceID = CreateMainEngine(windowID);

    Engine::Engine* engineInstance = Core::Application::GetEngineInstance(engineInstanceID);
    engineInstance->CreateSubProc<EngineSubProc>();
}
```
# Everything Else
Links to all of the docs
## Core
- [Core](Core/Core.md)
- [Entry Point](Core/EntryPoint.md)
- [ID Handler](Core/IdHandler.md)
- [ImGui](Core/ImGui.md)
- [System](Core/System.md)
- [Window](Core/Window.md)
#### Event
- [Event](Core/Event/Event.md)
- [Input Events](Core/Event/InputEvent.md)
- [Window Events](Core/Event/WindowEvent.md)
#### Processes
- [Application](Core/Process/Application.md)
- [Sub Process](Core/Process/SubProcess.md)
#### Utilities
- [Logger](Core/Utilities/Logger.md)
## Engine
- [Engine](Engine/Engine.md)
## Renderer
- [Renderer](Renderer/Renderer.md)