#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"
#include "Enigma/Renderer/Texture.h"
#include <vector>

namespace Enigma::Renderer {

	class VertexBuffer {
	public:
		static ref<VertexBuffer> Create(const std::vector<DataType>& layout, Usage usage);

		virtual void InitAttribs() = 0;

		virtual void SetData(void* vertices, u64 size) = 0;
		virtual void SetSubData(void* vertices, u64 size, u32 offset) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
	};

	class IndexBuffer {
	public:
		static ref<IndexBuffer> Create(DataType type, Usage usage);

		virtual u32 GetIndexCount() = 0;
		virtual DataType GetIndexType() = 0;

		virtual void SetData(void* indices, u64 size) = 0;
		virtual void SetSubData(void* indices, u64 size, u32 offset) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
	};
	
	class UniformBuffer {
	public:
		static ref<UniformBuffer> Create(u32 size, u32 binding, Usage usage);

		virtual void SetData(void* data, u32 size, u32 offset) = 0;
	
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
		u32 width;
		u32 height;

		// color textures will be attached in the order given here
		// depth attachment must be at the end of the list if it exists
		std::vector<Attachment> attachments;
	};
	class FrameBuffer {
	public:
		static ref<FrameBuffer> Create(const FrameBufferConfig & config);

		virtual void Resize(u32 width, u32 height) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual std::vector<ref<Texture>> GetAttachments() = 0;
		virtual ref<Texture> GetColorAttachment(u32 index) = 0;
		// Returns the current color attachment at index then creates a new color attachment
		virtual ref<Texture> SeverColorAttachment(u32 index) = 0;
		virtual ref<Texture> GetDepthAttachment() = 0;

		virtual void GetPixel(u32 x, u32 y, u32 attachment, void* data) = 0;
	};

}