#version 450 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoord;

layout (std140, binding = 0) uniform Camera {
    mat4 viewProjection;
} u_Camera;

uniform mat4 u_Model;

struct VertexOutput {
    vec2 texCoord;
};
layout (location = 0) out VertexOutput Output;

void main() {
    Output.texCoord = a_TexCoord;
    gl_Position = u_Camera.viewProjection * u_Model * vec4(a_Pos, 1.0);
}