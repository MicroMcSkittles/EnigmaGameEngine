#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"
#include "Enigma/Renderer/Texture.h"
#include <vector>

namespace Enigma {
	namespace Renderer {

		class VertexBuffer {
		public:
			static ref<VertexBuffer> Create(const std::vector<DataType>& layout, Usage usage);

			virtual void InitAttribs() = 0;

			virtual void SetData(void* vertices, i32 size) = 0;
			virtual void SetSubData(void* vertices, i32 size, i32 offset) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;
		};

		class IndexBuffer {
		public:
			static ref<IndexBuffer> Create(DataType type, Usage usage);

			virtual int GetIndexCount() = 0;
			virtual DataType GetIndexType() = 0;

			virtual void SetData(void* indices, i32 size) = 0;
			virtual void SetSubData(void* indices, i32 size, i32 offset) = 0;

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

			ref<Texture> output; // If output is not nullptr, then it will be used as the attachment

			// used when creating texture
			TexFormat format = TexFormat::RGB;
			// used when creating texture
			TexFormat internalFormat = TexFormat::RGB;
			// used when creating color texture
			DataType dataType = DataType::UnsignedByte;
		};
		struct FrameBufferConfig{
			i32 width;
			i32 height;

			// color textures will be attached in the order given here
			// depth attachment must be at the end of the list if it exists
			std::vector<Attachment> attachments;
		};
		class FrameBuffer {
		public:
			static ref<FrameBuffer> Create(const FrameBufferConfig & config);

			virtual void Resize(i32 width, i32 height) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual std::vector<ref<Texture>> GetAttachments() = 0;
			virtual ref<Texture> GetColorAttachment(i32 index) = 0;
			// Returns the current color attachment at index then creates a new color attachment
			virtual ref<Texture> SeverColorAttachment(i32 index) = 0;
			virtual ref<Texture> GetDepthAttachment() = 0;
		};
	}
}