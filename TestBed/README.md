# Test Bed
The Test Bed project is used to test new features. Its nothing too complex.

# How To Use
Create a class that inherits from the base class `TestContext`.
| Function | Description |
| -------- | ----------- |
| `TestContext(Core::ID windowID)` | The constructor |
| `virtual void OnEvent(Enigma::Core::Event& e) { }` | Called every time an event occurs |
| `virtual void Update(Enigma::Engine::DeltaTime deltaTime) { }` | Called every frame |
| `virtual void Render() { }` | Called when the application is rendering |
| `virtual void ImGui() { }` | Called when the application is processing [ImGui](../Docs/Enigma/Core/ImGui.md) |

Implement any of the virtual functions needed for your test.
The TestBed class automaticaly call the constructor for your test class.
You must go to the [TestBed.cpp](src/TestBed.cpp) file and include your test and define the name of your test class under the name `CURRENT_TEST_CONTEXT`.

## Example
this would be in the [TestBed.cpp](src/TestBed.cpp) file.
``` c++
class FontTestContext : public TestContext {
public:
    FontTestContext(Core::ID windowID) : TestContext(windowID) { }

    virtual void OnEvent(Core::Event& e) override { /* ... */ }
    virtual void Update(Engine::DeltaTime deltaTime) override { /* ... */ }
    virtual void Render() override { /* ... */ }
    virtual void ImGui() override { /* ... */ }

    // everything else ...
}

#define CURRENT_TEST_CONTEXT FontTestContext
```