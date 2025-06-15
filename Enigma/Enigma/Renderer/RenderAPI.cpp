#include "Renderer/RenderAPI.h"
#include "Core/Core.h"

#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Enigma {
    namespace Renderer {

        void RenderAPI::Init(API api) {
            switch (api)
            {
            case API::OpenGL: 
                s_Instance = new Platform::OpenGL::OpenGLRenderAPI();
                break;
            case API::Vulkan: LOG_ERROR("Vulkan hasn't been impl yet!"); break;
            default:
                LOG_ERROR("Unknown renderer API");
            }
        }

    }
}
