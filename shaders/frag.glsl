#version 460 core

struct fs_input {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

layout (location = 0) in fs_input frag_in;
layout (location = 1) in vec3 frag_pos;
layout (location = 2) in vec3 view_in;

layout (location = 3) uniform sampler2D tex_1;

layout (location = 0) out vec4 color;

float D_GGX(float NoH, float roughness) {
    float roughness2 = roughness * roughness;
    float f = (NoH * roughness2 - NoH) * NoH + 1.0f;
    return roughness2 / (3.14f * f * f);
}

vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0f) - f0) * pow(1.0f - u, 5.0f);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}

vec3 BRDF(vec3 l,vec3 v, vec3 n, vec3 diffuse_color , float perceptual_roughness, vec3 f0) {
    vec3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    // perceptually linear roughness to roughness (see parameterization)
    float roughness = perceptual_roughness * perceptual_roughness;

    float D = D_GGX(NoH, roughness);
    vec3  F = F_Schlick(LoH, f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    // specular BRDF
    vec3 Fr = (D * V) * F;

    // diffuse BRDF
    vec3 Fd = diffuse_color * V_SmithGGXCorrelated(NoV, NoL, roughness);

    // apply lighting...
    return Fr + Fd;
}

void main() {
    vec3 light_direction = normalize(frag_pos);
    float roughness = 0.4;
    
    vec3 frfd = BRDF(light_direction, view_in, frag_in.normal, vec3(0.3f, 0.3f, 0.3f), roughness, vec3(0.78f, 0.71f, 0.77f));

    vec3 out_color = vec3(texture(tex_1, frag_in.uv)) * frfd;

    color = vec4(out_color, 1.0f);
}