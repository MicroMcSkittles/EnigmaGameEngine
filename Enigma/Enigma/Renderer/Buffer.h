#pragma once
#include "Renderer/RenderEnum.h"

#include <vector>

namespace Enigma {
	namespace Renderer {

		class VertexBuffer {
		public:
			static VertexBuffer* Create(const std::vector<DataType>& layout, Usage usage);

			virtual void SetData(void* vertices, int size) = 0;
			virtual void Initialize() = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

		class IndexBuffer {
		public:
			static IndexBuffer* Create(DataType type, Usage usage);

			virtual int GetIndexCount() = 0;
			virtual DataType GetIndexType() = 0;

			virtual void SetData(unsigned int* indices, int size) = 0;
			virtual void Initialize() = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};
	}
}