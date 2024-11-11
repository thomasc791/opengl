#version 440 core

struct Ant {
    vec2 pos;
    float dir;
    float u;
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
layout(location = 3) uniform float dist;
layout(location = 4) uniform float angle;
layout(location = 5) uniform float turnAngle;

void move(inout Ant antCurr) {
    antCurr.pos.x -= texSize.x * sign(antCurr.pos.x);
    antCurr.pos.y -= texSize.y * sign(antCurr.pos.y);

    // Calculating the values ahead of the ant
    float cosL = cos(antCurr.dir + angle);
    float cosR = cos(antCurr.dir - angle);
    float sinL = sin(antCurr.dir + angle);
    float sinR = sin(antCurr.dir - angle);
    float lookFront = length(antImage[int(antCurr.pos.x + cos(antCurr.dir) + texSize.x * antCurr.pos.y + sin(antCurr.dir))].u);
    float lookLeft = length(int((antCurr.pos.x + cosL * dist) + texSize.x * (antCurr.pos.y + sinL * dist)));
    float lookRight = length(antImage[int((antCurr.pos.x + cosR * dist) + texSize.x * (antCurr.pos.y + sinR * dist))].u);

    bool left = lookLeft >= lookFront && lookLeft >= lookRight;
    bool right = lookRight >= lookFront && lookRight >= lookLeft;

    antCurr.dir += -turnAngle * (float(int(right)) - float(int(left)));
    antCurr.pos += (vel * vec2(cos(antCurr.dir), sin(antCurr.dir)));

    antCurr.color.x = float(int(left));
    antCurr.color.y = float(dist / 20);
    antCurr.color.z = float(int(right));
}

void main() {
    uint curr = gl_GlobalInvocationID.x;

    move(ant[curr]);
    ivec2 texelCoord = ivec2(ant[curr].pos);
    antImage[texelCoord.y * texSize.x + texelCoord.x].u = vec4(1.0);
}
