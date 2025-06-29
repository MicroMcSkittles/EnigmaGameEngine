#pragma once
#include "Renderer/RenderEnum.h"

#include <glm/glm.hpp>

namespace Enigma {
    namespace Renderer {

        class RenderAPI {
        public:
            static RenderAPI* CreateContext(API api);
            static void MakeContextCurrent(RenderAPI* context);

            static API GetAPI() { return s_CurrentContext->GetAPIImpl(); }

            static void SetClearColor(const glm::vec4& color) { s_CurrentContext->SetClearColorImpl(color); }

            static void SetClearMask(uint32_t mask) { s_CurrentContext->SetClearMaskImpl(mask); }
            static void Clear() { s_CurrentContext->ClearImpl(); }

            static glm::vec2 GetViewportSize() { return s_CurrentContext->GetViewportSizeImpl(); }
            static void SetViewport(int width, int height) { s_CurrentContext->SetViewportImpl(width, height); }

            static void SetDrawMode(DrawMode mode) { s_CurrentContext->SetDrawModeImpl(mode); }
            static void DrawIndexed(int count, DataType type, void* data) { s_CurrentContext->DrawIndexedImpl(count, type, data); }
            static void DrawArrays(int first, int count) { s_CurrentContext->DrawArraysImpl(first, count); }

        protected:
            virtual API GetAPIImpl() = 0;

            virtual void SetClearColorImpl(const glm::vec4& color) = 0;

            virtual void SetClearMaskImpl(uint32_t mask) = 0;
            virtual void ClearImpl() = 0;

            virtual glm::vec2 GetViewportSizeImpl() = 0;
            virtual void SetViewportImpl(int width, int height) = 0;

            virtual void SetDrawModeImpl(DrawMode mode) = 0;
            virtual void DrawIndexedImpl(int count, DataType type, void* data) = 0;
            virtual void DrawArraysImpl(int first, int count) = 0;

        private:
            inline static RenderAPI* s_CurrentContext;
        };

    }
}