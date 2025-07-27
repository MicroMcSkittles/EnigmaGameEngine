#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform int StencilID;
const float Epsilon = 0.01;

void main() {

    // Find the current point on the circle
    vec2 point = TexCoord * 2 - 1;
    // Find the distance from the point to the center of the circle
    // Using length because we assume the circle is a unit circle at 0 0
    // This means the the distance to the center is the length of point
    float dist = 1 - length(point);

    // Check of point is in the circle by checking if the distance is less then the radius
    //float stencil = smoothstep(0.0, 0.005, dist);
    float stencil = step(0.0, dist);
    if(stencil < Epsilon) discard;

    int rslts = int(floor(stencil * 0xFF));
    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);

    FragColor = vec4(rslt);
}