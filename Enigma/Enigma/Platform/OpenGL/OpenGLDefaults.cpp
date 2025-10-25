#include "Enigma/Renderer/Renderer2D/Render2D.h"
#include "Enigma/Renderer/DefaultShaders.h"

namespace Enigma {
	namespace Renderer {

		// Render2D default shaders

		ref<Shader> DefaultShaders::LoadMainShader() {
			/*const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\nout vec2 FragPos;\n\nuniform mat4 ViewProjection;\nuniform mat4 Model;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    vec4 position = ViewProjection * Model * vec4(aPos, 1.0);\n    FragPos = (position.xy + vec2(1, 1)) / 2;\n    gl_Position = position;\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\nin vec2 FragPos;\n\nuniform sampler2D TextureMap;\nuniform sampler2D StencilMap;\nuniform int StencilID;\nuniform vec4 Tint;\nconst float Epsilon = 1.0 / 255.0;\n\nvoid main() {\n    float stencil = 0;\n    if(StencilID != -1) {\n        int data = int(texture(StencilMap, FragPos).r);\n        int s = data & 0xFF;\n        int id = (data >> 8) & 0xFFFFFF;\n        stencil = float(s) / 255.f;\n        if(id != StencilID || stencil < Epsilon) discard;\n    }\n    else {\n        stencil = 1;\n    }\n\n    vec4 textureColor = texture(TextureMap, TexCoord);\n    if (textureColor.a < 0.5) discard;\n    FragColor = textureColor * stencil * Tint;\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);*/

			Renderer::ShaderConfig config;
			config.vertexPath = "../Enigma/DefaultShaders/SceneViewShader.vert";
			config.pixelPath = "../Enigma/DefaultShaders/SceneViewShader.frag";
			return Renderer::Shader::Create(config);
		}

		ref<Shader> DefaultShaders::LoadCircleShader()
		{
			Renderer::ShaderConfig config;
			config.vertexPath = "../Enigma/DefaultShaders/SceneViewShader.vert";
			config.pixelPath = "../Enigma/DefaultShaders/CircleShader.frag";
			return Renderer::Shader::Create(config);
		}

		ref<Shader> Renderer::DefaultShaders::LoadPostProcShader() {
			/*const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    gl_Position = vec4(aPos, 1.0);\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\n\nuniform sampler2D FrameTexture;\n\nvoid main() {\n    vec4 frameColor = texture(FrameTexture, TexCoord);\n    //frameColor = 1.0 - frameColor;\n    FragColor = frameColor;\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);*/

			Renderer::ShaderConfig config;
			config.vertexPath = "../Enigma/DefaultShaders/PostProcessShader.vert";
			config.pixelPath = "../Enigma/DefaultShaders/PostProcessShader.frag";
			return Renderer::Shader::Create(config);
		}

		ref<Shader> Render2D::LoadCircleStencilShader()
		{
			const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\n\nuniform mat4 ViewProjection;\nuniform mat4 Model;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\n\nuniform int StencilID;\nconst float Epsilon = 0.01;\n\nvoid main() {\n\n    vec2 point = TexCoord * 2 - 1;\n    float dist = 1 - length(point);\n    float stencil = step(0.0, dist);\n    if(stencil < Epsilon) discard;\n\n    int rslts = int(floor(stencil * 0xFF));\n    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);\n\n    FragColor = vec4(rslt);\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);
		}

		ref<Shader> Render2D::LoadLineCircleStencilShader()
		{
			const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\n\nuniform mat4 ViewProjection;\nuniform mat4 Model;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\n\nuniform float Thickness;\n\nuniform int StencilID;\nconst float Epsilon = 0.01;\n\nvoid main() {\n\n    vec2 point = TexCoord * 2 - 1;\n    float dist = length(point);\n\n    float stencil = 0;\n    if(dist > 1 - Thickness && dist < 1) stencil = 1;\n\n    if(stencil < Epsilon) discard;\n\n    int rslts = int(floor(stencil * 0xFF));\n    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);\n\n    FragColor = vec4(rslt);\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);
		}

		ref<Shader> Render2D::LoadLineQuadStencilShader()
		{
			const char* vertSource = "#version 460 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 TexCoord;\n\nuniform mat4 ViewProjection;\nuniform mat4 Model;\n\nvoid main() {\n    TexCoord = aTexCoord;\n    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);\n}";
			const char* fragSource = "#version 460 core\nout vec4 FragColor;\n\nin vec2 TexCoord;\n\nuniform float Thickness;\nuniform vec2 Bounds;\n\nuniform int StencilID;\nconst float Epsilon = 0.01;\n\nvoid main() {\n\n    vec2 point = TexCoord * 2 - 1;\n    point *= Bounds;\n    float rThickness = Thickness * 0.25;\n\n    float stencil = 0;\n    if(point.x < -Bounds.x + rThickness || point.x > Bounds.x - rThickness) stencil = 1;\n    else if(point.y < -Bounds.y + rThickness || point.y > Bounds.y - rThickness) stencil = 1;\n\n    if(stencil < Epsilon) discard;\n\n    int rslts = int(floor(stencil * 0xFF));\n    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);\n\n    FragColor = vec4(rslt);\n}";

			Renderer::ShaderConfig config;
			config.flags |= Renderer::ShaderFlag::PathAsSource;
			config.vertexPath = vertSource;
			config.pixelPath = fragSource;
			return Renderer::Shader::Create(config);
		}

		ref<Shader> Render2D::LoadTextStencilShader()
		{
			Renderer::ShaderConfig config;
			config.vertexPath = "../Enigma/DefaultShaders/TextStencilShader.vert";
			config.pixelPath = "../Enigma/DefaultShaders/TextStencilShader.frag";
			return Renderer::Shader::Create(config);
		}

	}
}