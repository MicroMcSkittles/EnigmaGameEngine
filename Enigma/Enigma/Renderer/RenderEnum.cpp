#include "Enigma/Renderer/RenderEnum.h"
#include "Enigma/Core/Core.h"
#include <sstream>

namespace Enigma {
    namespace Renderer {
        std::string ToString(API api)
        {
            switch (api)
            {
            case Enigma::Renderer::API::OpenGL: return "OpenGL";
            case Enigma::Renderer::API::Vulkan: return "Vulkan";
            }
            return std::string();
        }
        std::string ToString(ClearMask mask)
        {
            std::stringstream ss;
            ss << ((mask & ClearMask::ColorBufferBit) ? "( Color Buffer Bit ) " : "");
            ss << ((mask & ClearMask::DepthBufferBit) ? "( Depth Buffer Bit ) " : "");
            ss << ((mask & ClearMask::StencilBufferBit) ? "( Stencil Buffer Bit )" : "");
            return ss.str();
        }
        std::string ToString(DrawMode mode)
        {
            switch (mode)
            {
            case Enigma::Renderer::DrawMode::Points: return "Points";
            case Enigma::Renderer::DrawMode::Patches: return "Patches";
            case Enigma::Renderer::DrawMode::Lines: return "Lines";
            case Enigma::Renderer::DrawMode::LineLoop: return "Line Loop";
            case Enigma::Renderer::DrawMode::LineStrip: return "Line Strip";
            case Enigma::Renderer::DrawMode::LinesAdjacency: return "Lines Adjacency";
            case Enigma::Renderer::DrawMode::LineStripAdjacency: return "Line Strip Adjacency";
            case Enigma::Renderer::DrawMode::Triangles: return "Triangles";
            case Enigma::Renderer::DrawMode::TriangleFan: return "Triangle Fan";
            case Enigma::Renderer::DrawMode::TriangleStrip: return "Triangle Strip";
            case Enigma::Renderer::DrawMode::TriangleStripAdjacency: return "Triangle Strip Adjacency";
            case Enigma::Renderer::DrawMode::TrianglesAdjacency: return "Triangles Adjacency";
            }
            return std::string();
        }
        std::string ToString(DataType type)
        {
            switch (type)
            {
            case Enigma::Renderer::DataType::UnsignedByte: return "Unsigned Byte";
            case Enigma::Renderer::DataType::UnsignedShort: return "Unsigned Short";
            case Enigma::Renderer::DataType::UnsignedInt: return "Unsigned Int";
            case Enigma::Renderer::DataType::UnsignedInt_24_8: return "Unsigned Int 24 8";
            case Enigma::Renderer::DataType::Int: return "Int";
            case Enigma::Renderer::DataType::Int2: return "Int 2";
            case Enigma::Renderer::DataType::Int3: return "Int 3";
            case Enigma::Renderer::DataType::Int4: return "Int 4";
            case Enigma::Renderer::DataType::Float: return "Float";
            case Enigma::Renderer::DataType::Float2: return "Float 2";
            case Enigma::Renderer::DataType::Float3: return "Float 3";
            case Enigma::Renderer::DataType::Float4: return "Float 4";
            case Enigma::Renderer::DataType::Matrix2: return "Matrix 2";
            case Enigma::Renderer::DataType::Matrix3: return "Matrix 3";
            case Enigma::Renderer::DataType::Matrix4: return "Matrix 4";
            case Enigma::Renderer::DataType::Texture2D: return "Texture 2D";
            }
            return std::string();
        }
        std::string ToString(Usage type)
        {
            switch (type)
            {
            case Enigma::Renderer::Usage::StaticDraw: return "Static Draw";
            case Enigma::Renderer::Usage::StaticRead: return "Static Read";
            case Enigma::Renderer::Usage::StaticCopy: return "Static Copy";
            case Enigma::Renderer::Usage::DynamicDraw: return "Dynamic Draw";
            case Enigma::Renderer::Usage::DynamicRead: return "Dynamic Read";
            case Enigma::Renderer::Usage::DynamicCopy: return "Dynamic Copy";
            case Enigma::Renderer::Usage::StreamDraw: return "Stream Draw";
            case Enigma::Renderer::Usage::StreamRead: return "Stream Read";
            case Enigma::Renderer::Usage::StreamCopy: return "Stream Copy";
            }
            return std::string();
        }
        std::string ToString(TexFormat format)
        {
            switch (format)
            {
            case Enigma::Renderer::TexFormat::RED: return "Red";
            case Enigma::Renderer::TexFormat::RG: return "RG";
            case Enigma::Renderer::TexFormat::RGB: return "RGB";
            case Enigma::Renderer::TexFormat::RGBA: return "RGBA";
            case Enigma::Renderer::TexFormat::DEPTH24_STENCIL8: return "Depth 24 Stencil 8";
            case Enigma::Renderer::TexFormat::DEPTH_STENCIL: return "Depth Stencil";
            }
            return std::string();
        }
        std::string ToString(TexFilterMode mode)
        {
            switch (mode)
            {
            case Enigma::Renderer::TexFilterMode::Nearest: return "Nearest";
            case Enigma::Renderer::TexFilterMode::Linear: return "Linear";
            }
            return std::string();
        }
        std::string ToString(TexWrapMode mode)
        {
            switch (mode)
            {
            case Enigma::Renderer::TexWrapMode::Repeat: return "Repeat";
            case Enigma::Renderer::TexWrapMode::MirroredRepeat: return "Mirrored Repeat";
            case Enigma::Renderer::TexWrapMode::ClampToEdge: return "Clamp To Edge";
            case Enigma::Renderer::TexWrapMode::ClampToBorder: return "Clamp To Border";
            }
            return std::string();
        }
    }
}