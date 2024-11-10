#version 440 core

struct Image {
    vec4 u;
    vec4 uPrev;
};

layout(rgba32f) uniform image2D imgOutput;
layout(binding = 1, std430) buffer AntImage {
    Image antImage[];
};

layout(location = 1) uniform uvec2 texSize;

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;
void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    uint j = uint(floor(gl_GlobalInvocationID.x / texSize.x));
    uint i = gl_GlobalInvocationID.x % texSize.x;
    ivec2 texelCoord = ivec2(i, j);
    uint curr = gl_GlobalInvocationID.x;

    antImage[curr].u = 0.99 * (antImage[curr].uPrev + 0.25 * (antImage[curr - 1].uPrev + antImage[curr + 1].uPrev + antImage[curr + texSize.x].uPrev + antImage[curr - texSize.x].uPrev - 4 * antImage[curr].uPrev));

    value = antImage[curr].u;

    imageStore(imgOutput, texelCoord, value);
}
