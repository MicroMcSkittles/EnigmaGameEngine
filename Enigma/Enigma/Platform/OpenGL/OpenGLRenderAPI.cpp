#include "Platform/OpenGL/OpenGLRenderAPI.h"
#include "Core/Core.h"

namespace Enigma {
    namespace Platform {
        namespace OpenGL {
            OpenGLRenderAPI::OpenGLRenderAPI() {
                LOG_MESSAGE("Initialized OpenGL!!", 4);
            }

            void OpenGLRenderAPI::SetClearColorImpl(const glm::vec4& color) {

            }

            void OpenGLRenderAPI::SetClearMaskImpl(uint32_t mask) {

            }
            void OpenGLRenderAPI::ClearImpl() {

            }

            void OpenGLRenderAPI::SetViewportImpl(int width, int height) {

            }

            void OpenGLRenderAPI::SetDrawModeImpl(Renderer::DrawMode mode) {

            }
            void OpenGLRenderAPI::DrawIndexedImpl(int count, Renderer::DataType type, void* data) {

            }
            void OpenGLRenderAPI::DrawArraysImpl(int first, int count) {

            }
        }
    }
}