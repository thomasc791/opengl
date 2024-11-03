#version 440 core

struct Point {
    vec2 pos;
    float angle;
    float radius;
    vec3 c;
    float hasFood;
};

struct Wave {
    float uPrev;
    float u;
    float uNext;
    float _padding;
};

layout(local_size_x = 128, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f) uniform image2D imgOutput;
layout(binding = 3, std430) buffer WaveArray {
    Wave data[];
};
layout(location = 0) uniform float t;
layout(location = 1) uniform uvec2 texSize;

void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    uint i = gl_GlobalInvocationID.x + 1;
    uint j = gl_GlobalInvocationID.y + 1;
    uint curr = j * texSize.x + i;

    data[curr].uNext = 1 * (2 * data[curr].u - data[curr].uPrev + 0.25 * (
                    data[curr - 1].u + data[curr + 1].u + data[(j - 1) * texSize.x + i].u + data[(j + 1) * texSize.x + i].u - 4 * data[curr].u));

    value.x = mix(1.0, 0.9, 10 * (data[curr].uNext)*(data[curr].uNext));
    value.y = mix(1.0, 0.0, 10 * abs(data[curr].uNext));
    value.z = mix(1.0, 0.4, 10 * abs(data[curr].uNext));

    imageStore(imgOutput, texelCoord, value);
}
