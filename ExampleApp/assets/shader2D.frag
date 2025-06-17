#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TextureMap;
uniform vec3 Tint;

void main() {
    FragColor = texture(TextureMap, TexCoord) * vec4(Tint, 1.0);
}