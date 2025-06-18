#pragma once
#include "Renderer/RenderEnum.h"
#include "Core/IdHandler.h"

#include <vector>

namespace Enigma {
	namespace Renderer {

		class VertexBuffer {
		public:
			static VertexBuffer* Create(const std::vector<DataType>& layout, Usage usage);
			Core::ID GetID() { return m_ID; }

			virtual void InitAttribs() = 0;

			virtual void SetData(void* vertices, int size) = 0;
			virtual void SetSubData(void* vertices, int size, int offset) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

		private:
			Core::ID m_ID;

		private:
			inline static Core::IDHandler<VertexBuffer> s_IDHandler;
		};

		class IndexBuffer {
		public:
			static IndexBuffer* Create(DataType type, Usage usage);
			Core::ID GetID() { return m_ID; }

			virtual int GetIndexCount() = 0;
			virtual DataType GetIndexType() = 0;

			virtual void SetData(void* indices, int size) = 0;
			virtual void SetSubData(void* indices, int size, int offset) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

		private:
			Core::ID m_ID;

		private:
			inline static Core::IDHandler<IndexBuffer> s_IDHandler;
		};
	}
}