#pragma once

#include "Core/Core.h"

namespace Enigma {
    namespace Renderer {
        enum class API {
            OpenGL,
            Vulkan
        };

        enum ClearMask {
            ColorBufferMask = BIT(0),
            DepthBufferMask = BIT(1),
            StencilBufferMask = BIT(2)
        };

        enum class DrawMode {
            Triangles,
            Lines
        };

        enum class DataType {
            UnsignedInt,
            UnsignedByte,
            Float,
            Float2,
            Float3,
            Float4,
            Int,
            Int2,
            Int3,
            Int4,
            Matrix2,
            Matrix3,
            Matrix4,
        };
    }
}