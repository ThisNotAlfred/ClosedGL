#version 460 core

layout( location = 0 ) in vec3 position_in;
layout( location = 1 ) in vec3 normal_in;
layout( location = 2 ) in vec2 uv_in;
layout( location = 3 ) in vec3 frag_pos;
layout( location = 4 ) in mat4 view_in;

out vec4 color_output;

layout(location = 3) uniform sampler2D tex_1;

vec3 light_pos = vec3(100.0f, 100.0f, 0.0f);

float fd_lambert() {
    vec3 N = normalize(normal_in);
    vec3 L = normalize(light_pos - frag_pos);
    return max(dot(N, L), 0.0f);
}

void main() {
    vec4 out_color = texture(tex_1, uv_in) * fd_lambert();

    color_output = out_color;
}