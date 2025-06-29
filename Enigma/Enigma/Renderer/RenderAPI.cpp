#include "Renderer/RenderAPI.h"
#include "Core/Core.h"

#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Enigma {
    namespace Renderer {

        RenderAPI* RenderAPI::CreateContext(API api)
        {
            switch (api)
            {
            case API::OpenGL: return new Platform::OpenGL::OpenGLRenderAPI();
            case API::Vulkan: LOG_ERROR("Vulkan hasn't been impl yet!"); break;
            default:
                LOG_ERROR("Unknown renderer API");
            }
        }

        void RenderAPI::MakeContextCurrent(RenderAPI* context)
        {
            if (s_CurrentContext == context) return;
            s_CurrentContext = context;
        }

    }
}
