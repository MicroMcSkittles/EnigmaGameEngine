#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"

#include <glm/glm.hpp>

namespace Enigma::Renderer {

    class RenderAPI {
    public:
        static ref<RenderAPI> CreateContext(API api);
        static void MakeContextCurrent(const ref<RenderAPI>& context);

        static API GetAPI() { return s_CurrentContext->GetAPIImpl(); }

        static void SetClearColor(const glm::vec4& color) { s_CurrentContext->SetClearColorImpl(color); }

        static void SetClearMask(u32 mask) { s_CurrentContext->SetClearMaskImpl(mask); }
        static void Clear() { s_CurrentContext->ClearImpl(); }

        static glm::vec2 GetViewportSize() { return s_CurrentContext->GetViewportSizeImpl(); }
        static void SetViewport(i32 width, i32 height) { s_CurrentContext->SetViewportImpl(width, height); }

        static void SetDrawMode(DrawMode mode) { s_CurrentContext->SetDrawModeImpl(mode); }
        static void DrawIndexed(i32 count, DataType type, void* data) { s_CurrentContext->DrawIndexedImpl(count, type, data); }
        static void DrawArrays(i32 first, i32 count) { s_CurrentContext->DrawArraysImpl(first, count); }

    protected:
        virtual API GetAPIImpl() = 0;

        virtual void SetClearColorImpl(const glm::vec4& color) = 0;

        virtual void SetClearMaskImpl(u32 mask) = 0;
        virtual void ClearImpl() = 0;

        virtual glm::vec2 GetViewportSizeImpl() = 0;
        virtual void SetViewportImpl(i32 width, i32 height) = 0;

        virtual void SetDrawModeImpl(DrawMode mode) = 0;
        virtual void DrawIndexedImpl(i32 count, DataType type, void* data) = 0;
        virtual void DrawArraysImpl(i32 first, i32 count) = 0;

    private:
        inline static ref<RenderAPI> s_CurrentContext;
    };
}