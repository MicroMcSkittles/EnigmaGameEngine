#pragma once
#include "Core/IdHandler.h"
#include "Renderer/Buffer.h"

namespace Enigma {
	namespace Renderer {

		class VertexArray {
		public:
			static VertexArray* Create();
			Core::ID GetID() { return m_ID; }

			virtual VertexBuffer* GetVertexBuffer() = 0;
			virtual IndexBuffer* GetIndexBuffer() = 0;

			virtual void AttachBuffer(VertexBuffer* vertexBuffer) = 0;
			virtual void AttachBuffer(IndexBuffer* indexBuffer) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

		private:
			Core::ID m_ID;

		private:
			inline static Core::IDHandler<VertexArray> s_IDHandle;
		};

	}
}