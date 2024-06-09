#version 460 core

out gl_PerVertex {
    vec4 gl_Position;
};

layout( location = 0 ) in vec3 position_in;
layout( location = 1 ) in vec3 normal_in;
layout( location = 2 ) in vec2 uv_in;

layout( location = 0 ) uniform mat4 projection;
layout( location = 1 ) uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(position_in, 1.0f);
}