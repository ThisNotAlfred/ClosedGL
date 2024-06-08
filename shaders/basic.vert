#version 460 core

out gl_PerVertex {
    vec4 gl_Position;
};

layout( location = 0 ) out vec3 position_out;
layout( location = 1 ) out vec3 normal_out;
layout( location = 2 ) out vec2 uv_out;
layout( location = 3 ) out vec3 frag_pos;
layout( location = 4 ) out vec3 view_out;

layout( location = 5 ) in vec3 position_in;
layout( location = 6 ) in vec3 normal_in;
layout( location = 7 ) in vec2 uv_in;

layout( location = 0 ) uniform mat4 projection;
layout( location = 1 ) uniform mat4 view;
layout( location = 2 ) uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position_in, 1.0f);


    position_out = position_in;
    normal_out = normal_in;
    uv_out = uv_in;

    frag_pos = vec3(model * vec4(position_in, 1.0f));
    view_out = vec3(frag_pos + view[3].xyz);
}