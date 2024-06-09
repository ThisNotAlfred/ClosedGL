#version 460 core

layout( location = 0 ) in vec3 position_in;
layout( location = 1 ) in vec3 normal_in;
layout( location = 2 ) in vec2 uv_in;
layout( location = 3 ) in vec3 frag_pos;
layout( location = 4 ) in mat4 view_in;

out vec4 color_output;

layout(location = 3) uniform vec3 cam_pos;
layout(location = 4) uniform sampler2D tex_1;

vec3 light_dir = vec3(0.0f, 1.0f, 1.0f);

const float GAMMA = 2.2;

void main() {
    vec3 view_dir = normalize(cam_pos - frag_pos);
    vec3 N = normalize(normal_in);
    vec3 L = light_dir;
    vec3 H = normalize(view_dir + light_dir);

    vec3 albedo = pow(texture(tex_1, uv_in).xyz, vec3(GAMMA));

    float phong = max(dot(H, N), 0.0);
    vec3 specular = vec3(pow(phong, 30.0));

    float lambertian = max(dot(L, N), 0.0f);
    vec3 color = lambertian * albedo + specular;

    vec3 reinhard = color / (color + vec3(1.0));
    vec3 corrected = pow(reinhard, vec3(1.0 / GAMMA));
    color_output = vec4(corrected, 1.0);
}