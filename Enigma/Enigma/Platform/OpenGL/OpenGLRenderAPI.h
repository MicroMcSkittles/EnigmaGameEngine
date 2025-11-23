#pragma once
#include "Enigma/Renderer/RenderAPI.h"

namespace Enigma::Platform::OpenGL {

    // Conversions for all the RendererEnums to OpenGL types.
    namespace Conversions {
        // Converts clear mask type to the OpenGL mask type
        u32 ClearMask(u32 mask);

        // Converts draw mode to the OpenGL draw mode
        u32 DrawMode(Renderer::DrawMode mode);

        // Converts data type to the OpenGL data type
        u32 DataType(Renderer::DataType type);
        // Converts OpenGL data type to data type
        Renderer::DataType DataType(u32 type);
        // Returns how many values are in a type
        u32 DataTypeCount(Renderer::DataType type);
        // Returns the size of data type in bytes
        u32 DataTypeSize(Renderer::DataType type);

        // Converts usage type to the OpenGL usage type
        u32 Usage(Renderer::Usage usage);

        // Converts texture format to OpenGL texture format
        u32 TexFormat(Renderer::TexFormat format);

        // Converts texture filter mode to OpenGL texture filter mode
        u32 TexFilterMode(Renderer::TexFilterMode mode);

        // Converts texture wrap mode to OpenGL texture wrap mode
        u32 TexWrapMode(Renderer::TexWrapMode mode);
    }

    class OpenGLRenderAPI : public Renderer::RenderAPI {
    public:
        OpenGLRenderAPI();

    protected:
        virtual Renderer::API GetAPIImpl() override { return Renderer::API::OpenGL; }

        virtual void SetClearColorImpl(const glm::vec4& color) override;

        virtual void SetClearMaskImpl(u32 mask) override;
        virtual void ClearImpl() override;

        virtual glm::vec2 GetViewportSizeImpl() override { return { (float)m_Data.viewportWidth, (float)m_Data.viewportHeight }; }
        virtual void SetViewportImpl(i32 width, i32 height) override;

        virtual void SetDrawModeImpl(Renderer::DrawMode mode) override;
        virtual void DrawIndexedImpl(i32 count, Renderer::DataType type, void* data) override;
        virtual void DrawArraysImpl(i32 first, i32 count) override;

    private:
        struct {
            u32 clearMask = 0;
            u32 drawMode  = 0;

            i32 viewportWidth  = 0;
            i32 viewportHeight = 0;
        } m_Data;
    };
}