#pragma once

#include "Core/Core.h"

namespace Enigma {
    namespace Renderer {
        enum class API {
            OpenGL,
            Vulkan
        };
		std::string ToString(API api);

		enum ClearMask {
			ColorBufferBit = BIT(0),
			DepthBufferBit = BIT(1),
			StencilBufferBit = BIT(2)
		};
		std::string ToString(ClearMask mask);

		enum class DrawMode {
			None = 0,
			Points,
			Patches,
			Lines,
			LineLoop,
			LineStrip,
			LinesAdjacency,
			LineStripAdjacency,
			Triangles,
			TriangleFan,
			TriangleStrip,
			TriangleStripAdjacency,
			TrianglesAdjacency
		};
		std::string ToString(DrawMode mode);

		enum class DataType {
			None = 0,
			UnsignedByte,
			UnsignedShort,
			UnsignedInt,
			UnsignedInt_24_8,
			Int,
			Int2,
			Int3,
			Int4,
			Float,
			Float2,
			Float3,
			Float4,
			Matrix2,
			Matrix3,
			Matrix4,
			Texture2D
		};
		std::string ToString(DataType type);

		enum class Usage {
			None = 0,
			StaticDraw,
			StaticRead,
			StaticCopy,
			DynamicDraw,
			DynamicRead,
			DynamicCopy,
			StreamDraw,
			StreamRead,
			StreamCopy,
		};
		std::string ToString(Usage type);

		enum class TexFormat
		{
			None = 0,
			RED,
			RG,
			RGB,
			RGBA,
			DEPTH24_STENCIL8,
			DEPTH_STENCIL,
		};
		std::string ToString(TexFormat format);

		enum class TexFilterMode
		{
			None = 0,
			Nearest,
			Linear
		};
		std::string ToString(TexFilterMode mode);
		
		enum class TexWrapMode
		{
			None = 0,
			Repeat,
			MirroredRepeat,
			ClampToEdge,
			ClampToBorder
		};
		std::string ToString(TexWrapMode mode);
    }
}