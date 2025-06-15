#pragma once
#include "Renderer/RenderEnum.h"

#include <glm/glm.hpp>

namespace Enigma {
    namespace Renderer {

        class RenderAPI {
        public:
            static void Init(API api);

            static void SetClearColor(const glm::vec4& color);

            static void SetClearMask(uint32_t mask);
            static void Clear();

            static void SetViewport(int width, int height);

            static void SetDrawMode(DrawMode mode);
            static void DrawIndexed(int count, DataType type, void* data);
            static void DrawArrays(int first, int count);

        protected:
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