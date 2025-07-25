#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D FrameTexture;

void main() {
    vec4 frameColor = texture(FrameTexture, TexCoord);
    //frameColor = 1.0 - frameColor;
    FragColor = frameColor;
}