#version 450 core
layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D FrameTexture;

struct VertexOutput {
    vec2 texCoord;
};
layout (location = 0) in VertexOutput Input;

void main() {    
    FragColor = texture(FrameTexture, Input.texCoord);
}