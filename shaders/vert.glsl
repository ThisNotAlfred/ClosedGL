#version 460 core

struct vs_output {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

layout (location = 0) out vs_output vert_output;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) uniform mat4 projection;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vert_output.position = position;
    vert_output.normal = normal;
    vert_output.uv = uv;
}