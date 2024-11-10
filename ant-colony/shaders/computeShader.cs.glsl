#version 440 core

struct Ant {
    vec2 pos;
    vec2 dir;
    vec4 color;
};

struct Image {
    vec4 u;
    vec4 uPrev;
};

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f) uniform image2D imgOutput;
layout(binding = 0, std430) buffer Ants {
    Ant ant[];
};
layout(binding = 1, std430) buffer AntImage {
    Image antImage[];
};

layout(location = 0) uniform float t;
layout(location = 1) uniform uvec2 texSize;
layout(location = 2) uniform float vel;

void move(inout Ant ant) {
        ant.pos.x -= texSize.x * sign(ant.pos.x);
        ant.pos.y -= texSize.y * sign(ant.pos.y);
    ant.pos += vel * ant.dir;
}

void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);

    uint j = uint(floor(gl_GlobalInvocationID.x / texSize.x));
    uint i = gl_GlobalInvocationID.x % texSize.x;
    uint curr = gl_GlobalInvocationID.x;

    move(ant[curr]);
    ivec2 texelCoord = ivec2(ant[curr].pos);
    value.x = 1.0;
    antImage[texelCoord.y * texSize.x + texelCoord.x].u = value;
}
