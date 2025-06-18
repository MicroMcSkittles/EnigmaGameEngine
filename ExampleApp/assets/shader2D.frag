#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TextureMap;
uniform vec3 Tint;

void main() {
    vec4 textureColor = texture(TextureMap, TexCoord);
    if (textureColor.a < 0.5) discard;
    FragColor = textureColor * vec4(Tint, 1.0);
}