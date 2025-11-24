#version 460 core
layout(location = 0) out int OutEntityID;

in vec2 TexCoord;

uniform int EntityID;

void main() {
    OutEntityID = EntityID + 1;
}