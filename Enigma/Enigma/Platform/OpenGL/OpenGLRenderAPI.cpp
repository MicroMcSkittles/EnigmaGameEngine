#include "Enigma/Platform/OpenGL/OpenGLRenderAPI.h"
#include "Enigma/Core/Core.h"
#include "Enigma/Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Enigma {
    namespace Platform {
        namespace OpenGL {
			namespace Conversions {
				uint32_t ClearMask(uint32_t mask) {
					uint32_t gl_mask = 0;
					gl_mask |= (mask & Renderer::ClearMask::ColorBufferBit) ? GL_COLOR_BUFFER_BIT : 0;
					gl_mask |= (mask & Renderer::ClearMask::DepthBufferBit) ? GL_DEPTH_BUFFER_BIT : 0;
					gl_mask |= (mask & Renderer::ClearMask::StencilBufferBit) ? GL_STENCIL_BUFFER_BIT : 0;
					return gl_mask;
				}

				uint32_t DrawMode(Renderer::DrawMode mode) {
					switch (mode)
					{
					case Renderer::DrawMode::Points: return GL_POINTS;
					case Renderer::DrawMode::Patches: return GL_PATCHES;
					case Renderer::DrawMode::Lines: return GL_LINES;
					case Renderer::DrawMode::LineLoop: return GL_LINE_LOOP;
					case Renderer::DrawMode::LineStrip: return GL_LINE_STRIP;
					case Renderer::DrawMode::LinesAdjacency: return GL_LINES_ADJACENCY;
					case Renderer::DrawMode::LineStripAdjacency: return GL_LINE_STRIP_ADJACENCY;
					case Renderer::DrawMode::Triangles: return GL_TRIANGLES;
					case Renderer::DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
					case Renderer::DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
					case Renderer::DrawMode::TrianglesAdjacency: return GL_TRIANGLES_ADJACENCY;
					case Renderer::DrawMode::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY;
					default:
						LOG_WARNING("Draw mode not supported");
						return 0;
					}
				}

				uint32_t DataType(Renderer::DataType type) {
					switch (type)
					{
					case Renderer::DataType::UnsignedByte:  return GL_UNSIGNED_BYTE;
					case Renderer::DataType::UnsignedShort: return GL_UNSIGNED_SHORT;
					case Renderer::DataType::UnsignedInt:   return GL_UNSIGNED_INT;
					case Renderer::DataType::UnsignedInt_24_8:     return GL_UNSIGNED_INT_24_8;
					case Renderer::DataType::Int:			  return GL_INT;
					case Renderer::DataType::Int2:		  return GL_INT_VEC2;
					case Renderer::DataType::Int3:		  return GL_INT_VEC3;
					case Renderer::DataType::Int4:		  return GL_INT_VEC4;
					case Renderer::DataType::Float:         return GL_FLOAT;
					case Renderer::DataType::Float2:        return GL_FLOAT;
					case Renderer::DataType::Float3:        return GL_FLOAT;
					case Renderer::DataType::Float4:        return GL_FLOAT;
					case Renderer::DataType::Matrix2:       return GL_FLOAT_MAT2;
					case Renderer::DataType::Matrix3:       return GL_FLOAT_MAT3;
					case Renderer::DataType::Matrix4:       return GL_FLOAT_MAT4;
					case Renderer::DataType::Texture2D:     return GL_SAMPLER_2D;
					default:
						LOG_WARNING("Unsupported data type");
						return 0;
					}
				}
				Renderer::DataType DataType(uint32_t type) {
					switch (type)
					{
					case GL_UNSIGNED_BYTE:  return Renderer::DataType::UnsignedByte;
					case GL_UNSIGNED_SHORT: return Renderer::DataType::UnsignedShort;
					case GL_UNSIGNED_INT:   return Renderer::DataType::UnsignedInt;
					case GL_INT:			return Renderer::DataType::Int;
					case GL_INT_VEC2:		return Renderer::DataType::Int2;
					case GL_INT_VEC3:		return Renderer::DataType::Int3;
					case GL_INT_VEC4:		return Renderer::DataType::Int4;
					case GL_FLOAT:          return Renderer::DataType::Float;
					case GL_FLOAT_VEC2:     return Renderer::DataType::Float2;
					case GL_FLOAT_VEC3:     return Renderer::DataType::Float3;
					case GL_FLOAT_VEC4:     return Renderer::DataType::Float4;
					case GL_FLOAT_MAT2:     return Renderer::DataType::Matrix2;
					case GL_FLOAT_MAT3:     return Renderer::DataType::Matrix3;
					case GL_FLOAT_MAT4:     return Renderer::DataType::Matrix4;
					case GL_SAMPLER_2D:     return Renderer::DataType::Texture2D;
					default:
						LOG_WARNING("OpenGL data type not supported ( %u )", type);
						return Renderer::DataType::None;
					}
				}
				uint32_t DataTypeCount(Renderer::DataType type) {
					switch (type)
					{
					case Renderer::DataType::UnsignedByte:  return 1;
					case Renderer::DataType::UnsignedShort: return 1;
					case Renderer::DataType::UnsignedInt:   return 1;
					case Renderer::DataType::Float:         return 1;
					case Renderer::DataType::Float2:        return 2;
					case Renderer::DataType::Float3:        return 3;
					case Renderer::DataType::Float4:        return 4;
					default:
						LOG_WARNING("Unsupported data type");
						return 0;
					}
				}
				uint32_t DataTypeSize(Renderer::DataType type) {
					switch (type)
					{
					case Renderer::DataType::UnsignedByte:  return sizeof(unsigned char) * 1;
					case Renderer::DataType::UnsignedShort: return sizeof(unsigned short) * 1;
					case Renderer::DataType::UnsignedInt:   return sizeof(unsigned int) * 1;
					case Renderer::DataType::Float:         return sizeof(float) * 1;
					case Renderer::DataType::Float2:        return sizeof(float) * 2;
					case Renderer::DataType::Float3:        return sizeof(float) * 3;
					case Renderer::DataType::Float4:        return sizeof(float) * 4;
					default:
						LOG_WARNING("Unsupported data type");
						return 0;
					}
				}

				uint32_t Usage(Renderer::Usage usage) {
					switch (usage)
					{
					case Renderer::Usage::StaticDraw: return GL_STATIC_DRAW;
					case Renderer::Usage::StaticRead: return GL_STATIC_READ;
					case Renderer::Usage::StaticCopy: return GL_STATIC_COPY;
					case Renderer::Usage::DynamicDraw: return GL_DYNAMIC_DRAW;
					case Renderer::Usage::DynamicRead: return GL_DYNAMIC_READ;
					case Renderer::Usage::DynamicCopy: return GL_DYNAMIC_COPY;
					case Renderer::Usage::StreamDraw: return GL_STREAM_DRAW;
					case Renderer::Usage::StreamRead: return GL_STREAM_READ;
					case Renderer::Usage::StreamCopy: return GL_STREAM_COPY;
					default:
						LOG_WARNING("Usage not supported");
						return 0;
					}
				}

				uint32_t TexFormat(Renderer::TexFormat format) {
					switch (format)
					{
					case Renderer::TexFormat::RED:              return GL_RED;
					case Renderer::TexFormat::RG:               return GL_RG;
					case Renderer::TexFormat::RGB:              return GL_RGB;
					case Renderer::TexFormat::RGBA:             return GL_RGBA;
					case Renderer::TexFormat::RED32F:           return GL_R32F;
					case Renderer::TexFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
					case Renderer::TexFormat::DEPTH_STENCIL:    return GL_DEPTH_STENCIL;
					default:
						LOG_WARNING("Format not supported");
						return 0;
					}
				}

				uint32_t TexFilterMode(Renderer::TexFilterMode mode) {
					switch (mode)
					{
					case Renderer::TexFilterMode::Nearest: return GL_NEAREST;
					case Renderer::TexFilterMode::Linear:  return GL_LINEAR;
					default:
						LOG_WARNING("Filter mode not supported");
						return 0;
					}
				}

				uint32_t TexWrapMode(Renderer::TexWrapMode mode) {
					switch (mode)
					{
					case Renderer::TexWrapMode::Repeat: return GL_REPEAT;
					case Renderer::TexWrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
					case Renderer::TexWrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
					case Renderer::TexWrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
					default:
						LOG_WARNING("Wrap mode not supported");
						return 0;
					}
				}
			}

            OpenGLRenderAPI::OpenGLRenderAPI() {

                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                    LOG_ERROR("Failed to initialize GLAD");
                    return;
                }

				glEnable(GL_DEPTH_TEST); // TODO: add an enable function to RenderAPI
				glLineWidth(1.f);

                LOG_MESSAGE("Initialized OpenGL!!", 2);
            }

            void OpenGLRenderAPI::SetClearColorImpl(const glm::vec4& color) {
                glClearColor(color.r, color.g, color.b, color.a);
                LOG_MESSAGE("OpenGL clear color set to ( %f, %f, %f, %f ).", 6, color.r, color.g, color.b, color.a);
            }

            void OpenGLRenderAPI::SetClearMaskImpl(uint32_t mask) {
				m_Data.clearMask = Conversions::ClearMask(mask);
				LOG_MESSAGE("OpenGL clear mask set to ( %s )", 6, Renderer::ToString((Renderer::ClearMask)mask).c_str());
            }
            void OpenGLRenderAPI::ClearImpl() {
                glClear(m_Data.clearMask);
            }

            void OpenGLRenderAPI::SetViewportImpl(int width, int height) {
                glViewport(0, 0, width, height);
				m_Data.viewportWidth = width;
				m_Data.viewportHeight = height;
                LOG_MESSAGE("OpenGL viewport set to ( %i, %i )", 6, width, height);
            }

            void OpenGLRenderAPI::SetDrawModeImpl(Renderer::DrawMode mode) {
				m_Data.drawMode = Conversions::DrawMode(mode);
				LOG_MESSAGE("OpenGL draw mode set to ( %s )", 6, Renderer::ToString(mode).c_str());
            }
            void OpenGLRenderAPI::DrawIndexedImpl(int count, Renderer::DataType type, void* data) {
				glDrawElements(m_Data.drawMode, count, Conversions::DataType(type), data);
            }
            void OpenGLRenderAPI::DrawArraysImpl(int first, int count) {
				glDrawArrays(m_Data.drawMode, first, count);
            }
        }
    }
}