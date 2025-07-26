#include "Enigma/Renderer/Renderer2D/Render2D.h"

namespace Enigma {
	namespace Renderer {

		// Render2D default shaders

		Renderer::Shader* Renderer::Render2D::LoadDefaultMainShader() {
			const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\n\nuniform mat4 ViewProjection;\nuniform mat4 Model;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\n\nuniform sampler2D TextureMap;\nuniform vec4 Tint;\n\nvoid main() {\n    vec4 textureColor = texture(TextureMap, TexCoord);\n    if (textureColor.a < 0.5) discard;\n    FragColor = Tint;\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);
		}

		Renderer::Shader* Renderer::Render2D::LoadDefaultPostProcShader() {
			const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    gl_Position = vec4(aPos, 1.0);\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\n\nuniform sampler2D FrameTexture;\n\nvoid main() {\n    vec4 frameColor = texture(FrameTexture, TexCoord);\n    //frameColor = 1.0 - frameColor;\n    FragColor = frameColor;\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);
		}

	}
}