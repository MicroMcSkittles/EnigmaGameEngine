#pragma once
#include "Enigma/Renderer/RenderEnum.h"
#include "Enigma/Renderer/Texture.h"
#include <vector>

namespace Enigma {
	namespace Renderer {

		class VertexBuffer {
		public:
			static VertexBuffer* Create(const std::vector<DataType>& layout, Usage usage);

			virtual void InitAttribs() = 0;

			virtual void SetData(void* vertices, int size) = 0;
			virtual void SetSubData(void* vertices, int size, int offset) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

		class IndexBuffer {
		public:
			static IndexBuffer* Create(DataType type, Usage usage);

			virtual int GetIndexCount() = 0;
			virtual DataType GetIndexType() = 0;

			virtual void SetData(void* indices, int size) = 0;
			virtual void SetSubData(void* indices, int size, int offset) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

		enum class AttachmentType {
			ColorAttachment,
			DepthAttachment,
			//StencilAttachment
		};
		struct Attachment {
			AttachmentType type;

			Texture* output = nullptr; // If output is not nullptr, then it will be used as the attachment

			// used when creating texture
			TexFormat format = TexFormat::RGB;
			// used when creating texture
			TexFormat internalFormat = TexFormat::RGB;
			// used when creating color texture
			DataType dataType = DataType::UnsignedByte;
		};
		struct FrameBufferConfig{
			int width;
			int height;

			// color textures will be attached in the order given here
			// depth attachment must be at the end of the list if it exists
			std::vector<Attachment> attachments;
		};
		class FrameBuffer {
		public:
			static FrameBuffer* Create(const FrameBufferConfig & config);

			virtual void Resize(int width, int height) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual std::vector<Texture*> GetAttachments() = 0;
			virtual Texture* GetColorAttachment(int index) = 0;
			// Returns the current color attachment at index then creates a new color attachment
			virtual Texture* SeverColorAttachment(int index) = 0;
			virtual Texture* GetDepthAttachment() = 0;
		};
	}
}