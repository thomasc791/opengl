#version 440 core

struct Image {
    vec4 u;
    vec4 uPrev;
};

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(binding = 1, std430) buffer AntImage {
    Image antImage[];
};

void main() {
    uint curr = gl_GlobalInvocationID.x;
    antImage[curr].uPrev = antImage[curr].u;
}
