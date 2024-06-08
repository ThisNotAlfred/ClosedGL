#version 460 core

layout( location = 0 ) in vec3 position_in;
layout( location = 1 ) in vec3 normal_in;
layout( location = 2 ) in vec2 uv_in;
layout( location = 3 ) in vec3 frag_pos;
layout( location = 4 ) in vec3 view_in;

out vec4 color_output;

layout(location = 3) uniform sampler2D tex_1;

void main() {
    vec3 light_direction = normalize(frag_pos);
    float roughness = 0.4;
    
    vec4 out_color = vec4(texture(tex_1, uv_in) * vec4(0.4, 0.4, 0.4, 0.4));

    color_output = vec4(out_color);
}