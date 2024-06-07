#version 460 core

struct fs_input {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

layout (location = 0) in fs_input frag_in;

layout (location = 0) out vec4 color;

vec3 directional_light = vec3(1.0f, 0.4f, 0.5f);

void main() {
    color = vec4(vec3(0.5f, 0.2f, 0.6f) * dot(directional_light, frag_in.normal), 1.0f);
}