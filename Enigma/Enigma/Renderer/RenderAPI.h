#pragma once
#include "Renderer/RenderEnum.h"

#include <glm/glm.hpp>

namespace Enigma {
    namespace Renderer {

        class RenderAPI {
        public:
            static void Init(API api);
            static API GetAPI() { return s_Instance->GetAPImpl(); }

            static void SetClearColor(const glm::vec4& color) { s_Instance->SetClearColorImpl(color); }

            static void SetClearMask(uint32_t mask) { s_Instance->SetClearMaskImpl(mask); }
            static void Clear() { s_Instance->ClearImpl(); }

            static void SetViewport(int width, int height) { s_Instance->SetViewportImpl(width, height); }

            static void SetDrawMode(DrawMode mode) { s_Instance->SetDrawModeImpl(mode); }
            static void DrawIndexed(int count, DataType type, void* data) { s_Instance->DrawIndexedImpl(count, type, data); }
            static void DrawArrays(int first, int count) { s_Instance->DrawArraysImpl(first, count); }

        protected:
            virtual API GetAPImpl() = 0;

            virtual void SetClearColorImpl(const glm::vec4& color) = 0;

            virtual void SetClearMaskImpl(uint32_t mask) = 0;
            virtual void ClearImpl() = 0;

            virtual void SetViewportImpl(int width, int height) = 0;

            virtual void SetDrawModeImpl(DrawMode mode) = 0;
            virtual void DrawIndexedImpl(int count, DataType type, void* data) = 0;
            virtual void DrawArraysImpl(int first, int count) = 0;

        private:
            inline static RenderAPI* s_Instance;
        };

    }
}