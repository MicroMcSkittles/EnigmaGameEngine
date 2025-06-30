# Application
The Application is a static class that handles everything about the Application.
## Functions
| Name | Description |
| ---- | ----------- |
| `Application(int argc, char** argv)` | Constructor |
| `~Application()` | Deconstructor |
| `Application* Get()` | Returns a pointer to the Application Instance
| `std::vector<std::string>& GetArguments()` | Returns all of the command line arguments
| `const std::string& GetArgument(int id)` | Returns the command line argument at id |
| `void Close()` | Compleates the current frame than closes the application
| `template<typename T> T* CreateSubProc()` | Creates and registers a [Sub Process](SubProcess.md) T then returns it |
| `void BindSubProcToWindow(SubProcess* proc, ID windowID)` | Binds a [Sub Process](SubProcess.md) (Proc) to a [Window](../Window.md) (windowID)  |
| `ID CreateEngineInstance(const Engine::EngineConfig& config)` | Creates and registers a [Engine Instance](../../Engine/Engine.md) then returns its [ID](../IdHandler.md) |
| `void DeleteEngineInstance(Engine::Engine* instance)` | Deletes a [Engine Instance](../../Engine/Engine.md) |
| `Engine::Engine* GetEngineInstance(ID id)` | Gets a [Engine Instance](../../Engine/Engine.md) from its [ID](../IdHandler.md) |
| `ID CreateWindow(const WindowConfig& config)` | Creates and registers a [Window](../Window.md)
| `ID CreateWindow(const WindowConfig& windowConfig, const ImGuiConfig& imguiConfig)` | Creates and registers a [Window](../Window.md) with [ImGui](../ImGui.md) |
| `Window* GetWindow(ID id)` | Gets a [Window](../Window.md) from its [ID](../IdHandler.md) |
| `ImGuiHandler* GetImGui(ID id)` | Gets a [ImGuiHandler](../ImGui.md) from its [ID](../IdHandler.md) |
| `void UseRenderAPI(Renderer::API api)` | Makes a [Render API](../../Renderer/Renderer.md) current |
| `Renderer::RenderAPI* GetRenderAPI(Renderer::API api)` | Gets a [Render API Context](../../Renderer/Renderer.md) |
| `void Run()` | Runs the Applications main loop |
## Private Members
Because Application is a static class all member variables are stored in a struct called `Data`. The struct `WindowHandler` helps manage all data revolving around [Windows](../Window.md).
### Window Handler Struct
| Name | Type | Description |
| ---- | ---- | ----------- |
| window | `Window*` | Pointer to the [Window](../Window.md) |
| imgui | `ImGuiHandler*` | Pointer to the ImGui instance. nullptr if it doesn't exist |
| engineInstances | `std::vector<ID>` | All of the [IDs](../IdHandler.md) to [Engine Instances](../../Engine/Engine.md) bound to the window|
| subProcesses | `std::vector<ID>` | All of the [IDs](../IdHandler.md) to [Sub Processes](SubProcess.md) bound to the window|
### Data Struct
| Name | Type | Description |
| ---- | ---- | ----------- |
| arguments | `std::vector<std::string>` | All commandline arguments
| isRunning | `bool` | True when the Application is running |
| deltaTime | `Engine::DeltaTime` | The amount of time the last frame took in miliseconds |
| subProcStack | `SubProcStack` | All [Sub Processes](SubProcess.md) |
| windows | `IDHandler<WindowHandler>` | All [Windows](../Window.md) |
| engineInstances | `IDHandler<Engine::Engine>` | All [Engine Instances](../../Engine/Engine.md) |
| renderAPIs | `std::map<Renderer::API, Renderer::RenderAPI*>` | All [Render APIs](../../Renderer/Renderer.md) |