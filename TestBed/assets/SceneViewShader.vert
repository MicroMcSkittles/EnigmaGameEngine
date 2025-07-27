#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 FragPos;

uniform mat4 ViewProjection;
uniform mat4 Model;

void main() {
    TexCoord = aTexCoord;
    vec4 position = ViewProjection * Model * vec4(aPos, 1.0);
    FragPos = (position.xy + vec2(1, 1)) / 2;
    gl_Position = position;
}