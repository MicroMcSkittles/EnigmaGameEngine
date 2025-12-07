#include "Enigma/EnigmaPCH.h"
#include "Enigma/Renderer/RenderAPI.h"
#include "Enigma/Core/Core.h"

#include "Enigma/Platform/OpenGL/OpenGLRenderAPI.h"

namespace Enigma::Renderer {

    ref<RenderAPI> RenderAPI::CreateContext(API api)
    {
        switch (api)
        {
        case API::OpenGL: return CreateRef<Platform::OpenGL::OpenGLRenderAPI>();
        case API::Vulkan: LOG_ERROR("Vulkan hasn't been impl yet!"); break;
        default: LOG_ERROR("Failed to create Render API Context, unknown Render API");
        }

        return nullptr;
    }

    void RenderAPI::MakeContextCurrent(const ref<RenderAPI>& context)
    {
        if (s_CurrentContext == context) return;
        s_CurrentContext = context;
    }

}
