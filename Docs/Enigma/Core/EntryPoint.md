# Entry Point
The Entry Point header file contains the main function and is responsible for creating the [Application](Process/Application.md) Instance.
It creates the Application Instance then calls the `ApplicationMain` function.
```C++
extern void Enigma::Core::ApplicationMain(Enigma::Core::Application* app);
```
It then calls the Application's `Run` function which starts the main loop.