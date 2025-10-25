#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderAPI.h"
#include "Enigma/Renderer/Buffer.h"

namespace Enigma {
	namespace Renderer {

		class VertexArray {
		public:
			static ref<VertexArray> Create();

			virtual ref<VertexBuffer> GetVertexBuffer() = 0;
			virtual ref<IndexBuffer> GetIndexBuffer() = 0;

			virtual void AttachBuffer(ref<VertexBuffer> vertexBuffer) = 0;
			virtual void AttachBuffer(ref<IndexBuffer> indexBuffer) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

	}
}