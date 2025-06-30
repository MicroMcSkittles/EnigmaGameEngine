# Sub Process
A Sub Process is a user defined process that can be bound to either the [Application](Application.md) or a [Engine Instance](../../Engine/Engine.md).
The Sub Process class is a abstract class with the following virtual functions.
| Name | Description |
| ---- | ----------- |
| `void StartUp()` | Called the first frame affter Sub Process is created |
| `void ShutDown()` | Called when Sub Process is destroyed |
| `bool OnEvent(Event& e)` | Called every time a event occures |
| `void Update(Engine::DeltaTime deltaTime)` | Called every frame |
| `void Render()` | Called when current bound [Engine](../../Engine/Engine.md) is rendering |
| `void ImGui()` | Called when current bound [Engine](../../Engine/Engine.md) is showing ImGui |

Some of these functions are only called when Sub Process is bound to a [Window](../Window.md) or [Engine](../../Engine/Engine.md). The following table shows the minimum requirements to call a function.
| Function | Window | Engine |
| -------- | ------ | ------ |
| `void StartUp()` | | |
| `void ShutDown()` | | |
| `bool OnEvent(Event& e)` | ✅ | |
| `void Update(Engine::DeltaTime deltaTime)` | | |
| `void Render()` | | ✅ |
| `void ImGui()` | ✅ | |

Sub Process also has some member variables
| Name | Type | Description |
| ---- | ---- | ----------- |
| m_Started | `bool` | If the `StartUp` function has been called yet |
| m_EngineID | `ID*` | A pointer to the bound [Engines](../../Engine/Engine.md) [ID](../IdHandler.md), nullptr if not bound to a [Engine](../../Engine/Engine.md)

The function `Engine::Engine* GetEngine()` is also provided. It returns a pointer to the bound [Engine](../../Engine/Engine.md), returns nullptr if there is no bound [Engine](../../Engine/Engine.md)
# Sub Process Stack
The `SubProcStack` class handles a list of [Sub Processes](#sub-process). It behaves very similarly to the [IDHandler](../IdHandler.md/#id-handler-class) class. It has the following functions.
| Name | Description |
| ---- | ----------- |
| `ID PushProcBack(SubProcess* subProc)` | Adds a [Sub Process](#sub-process) to the list and returns its [ID](../IdHandler.md/#id-struct) |
| `void RemoveProc(ID id)` | Removes a [Sub Process](#sub-process) from the list at [ID](../IdHandler.md/#id-struct) |
| `ID GetProcessID(SubProcess* proc)` | Gets the [ID](../IdHandler.md/#id-struct) of proc, returns [InvalidID](../IdHandler.md/#id-struct) if proc is not in the list |
| `std::vector<SubProcess*>& GetData()` | Returns a reference to the list of [Sub Processes](#sub-process) |
| `void OnEvent(Event& e)` | Calls the OnEvent function for every [Sub Process](#sub-process), goes through list form front to back |
| `void Update(Engine::DeltaTime deltaTime)` | Calls the Update function for every [Sub Process](#sub-process) |
| `void Render()` | Calls the Render function for every [Sub Process](#sub-process) |
| `void ImGui()` | Calls the ImGui function for every [Sub Process](#sub-process) |
| `void ShutDown()` | Calls the Shutdown function for every [Sub Process](#sub-process), then deletes every [Sub Process](#sub-process) |

`SubProcStack` has one member variable.
`IDHandler<SubProcess> m_SubProcesses`
It uses the [IDHandler](../IdHandler.md/#id-handler-class) class to store a list of [Sub Processes](#sub-process).