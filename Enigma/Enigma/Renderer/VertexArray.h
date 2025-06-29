#pragma once
#include "Renderer/RenderAPI.h"
#include "Renderer/Buffer.h"

namespace Enigma {
	namespace Renderer {

		class VertexArray {
		public:
			static VertexArray* Create();

			virtual VertexBuffer* GetVertexBuffer() = 0;
			virtual IndexBuffer* GetIndexBuffer() = 0;

			virtual void AttachBuffer(VertexBuffer* vertexBuffer) = 0;
			virtual void AttachBuffer(IndexBuffer* indexBuffer) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

	}
}