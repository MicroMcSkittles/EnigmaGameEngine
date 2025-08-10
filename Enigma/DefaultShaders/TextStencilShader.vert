#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aCharID;

out vec2 TexCoord;
out float CharID;

uniform mat4 ViewProjection;
uniform mat4 Model;

void main() {
    TexCoord = aTexCoord;
    CharID = aCharID;
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);
}