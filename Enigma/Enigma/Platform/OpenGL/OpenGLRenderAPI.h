#pragma once
#include "Enigma/Renderer/RenderAPI.h"

namespace Enigma {
    namespace Platform {
        namespace OpenGL {
            
            // Conversions for all the RendererEnums to OpenGL types.
            namespace Conversions {
                // Converts clear mask type to the OpenGL mask type
                uint32_t ClearMask(uint32_t mask);

                // Converts draw mode to the OpenGL draw mode
                uint32_t DrawMode(Renderer::DrawMode mode);

                // Converts data type to the OpenGL data type
                uint32_t DataType(Renderer::DataType type);
                // Converts OpenGL data type to data type
                Renderer::DataType DataType(uint32_t type);
                // Returns how many values are in a type
                uint32_t DataTypeCount(Renderer::DataType type);
                // Returns the size of data type in bytes
                uint32_t DataTypeSize(Renderer::DataType type);

                // Converts usage type to the OpenGL usage type
                uint32_t Usage(Renderer::Usage usage);

                // Converts texture format to OpenGL texture format
                uint32_t TexFormat(Renderer::TexFormat format);

                // Converts texture filter mode to OpenGL texture filter mode
                uint32_t TexFilterMode(Renderer::TexFilterMode mode);

                // Converts texture wrap mode to OpenGL texture wrap mode
                uint32_t TexWrapMode(Renderer::TexWrapMode mode);
            }

            class OpenGLRenderAPI : public Renderer::RenderAPI {
            public:
                OpenGLRenderAPI();

            protected:
                virtual Renderer::API GetAPIImpl() override { return Renderer::API::OpenGL; }

                virtual void SetClearColorImpl(const glm::vec4& color) override;

                virtual void SetClearMaskImpl(uint32_t mask) override;
                virtual void ClearImpl() override;

                virtual glm::vec2 GetViewportSizeImpl() override { return { (float)m_Data.viewportWidth, (float)m_Data.viewportHeight }; }
                virtual void SetViewportImpl(int width, int height) override;

                virtual void SetDrawModeImpl(Renderer::DrawMode mode) override;
                virtual void DrawIndexedImpl(int count, Renderer::DataType type, void* data) override;
                virtual void DrawArraysImpl(int first, int count) override;

            private:
                struct {
                    uint32_t clearMask = NULL;
                    uint32_t drawMode  = NULL;

                    int viewportWidth  = NULL;
                    int viewportHeight = NULL;
                } m_Data;
            };

        }
    }
}