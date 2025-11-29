# Task List

- [ ] Look at old render api its not great... make it better bro
- [x] Impl uniform buffers to remove uniforms from shaders this will help with the vulkan impl later.
- [ ] Add location binding to textures in shaders
- [ ] Switch to spir-v shaders this will make adding vulkan support easier it can be cross compiled to other apis so I could support anything from Vulkan and OpenGL to Metal and Direct-X
- [ ] Use the liberary by google "shaderc" for shader compiling don't right it yourself moron

## To summarize
To sumerize, Rework shader API to switch to Vulkan GLSL and cross compile with Spir-V to OpenGL GLSL