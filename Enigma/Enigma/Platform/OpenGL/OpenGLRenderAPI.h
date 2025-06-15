#pragma once
#include "Renderer/RenderAPI.h"

namespace Enigma {
    namespace Platform {
        namespace OpenGL {
            
            class OpenGLRenderAPI : public Renderer::RenderAPI {
            public:
                OpenGLRenderAPI();

            protected:
                virtual void SetClearColorImpl(const glm::vec4& color) override;

                virtual void SetClearMaskImpl(uint32_t mask) override;
                virtual void ClearImpl() override;

                virtual void SetViewportImpl(int width, int height) override;

                virtual void SetDrawModeImpl(Renderer::DrawMode mode) override;
                virtual void DrawIndexedImpl(int count, Renderer::DataType type, void* data) override;
                virtual void DrawArraysImpl(int first, int count) override;

            private:

            };

        }
    }
}