#version 460 core

struct Point {
    vec2 pos;
    float angle;
    float radius;
    vec3 c;
    float hasFood;
};

layout(local_size_x = 128, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f) uniform image2D imgOutput;
layout(binding = 0) buffer WaveArray {
    vec4[1920][1080] data;
} waveArray;
layout(location = 0) uniform float t;

void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    // value.x = float(texelCoord.x) / (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    // value.y = float(texelCoord.y) / (gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    value.x = waveArray.data[texelCoord.x][texelCoord.y].x;
    value.y = waveArray.data[texelCoord.x][texelCoord.y].y;
    value.z = waveArray.data[texelCoord.x][texelCoord.y].z;
    waveArray.data[texelCoord.x][texelCoord.y].x = waveArray.data[texelCoord.x][texelCoord.y].y;
    waveArray.data[texelCoord.x][texelCoord.y].y = waveArray.data[texelCoord.x][texelCoord.y].z;
    waveArray.data[texelCoord.x][texelCoord.y].z = waveArray.data[texelCoord.x][texelCoord.y].x;
    // waveArray.data[texelCoord.x][texelCoord.y].x = 1f;
    // value.z = waveArray.data[texelCoord.x][texelCoord.y].x;
    imageStore(imgOutput, texelCoord, value);
}
