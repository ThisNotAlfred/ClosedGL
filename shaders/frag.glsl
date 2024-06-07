#version 460 core

struct fs_input {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

layout (location = 0) in fs_input frag_in;

layout (location = 1) uniform mat4 view;
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

vec3 BRDF(vec3 l, mat4 view, vec3 n, vec3 diffuse_color , float perceptual_roughness, vec3 f0) {
    vec3 v = -view[3].xyz;
    
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
    vec3 out_color = vec3(0.5f, 0.5f, 0.5f);
    vec3 directional_light = vec3(1.0f, 0.4f, 0.5f);
    float roughness = 0.4;
    
    // vec3 frfd = BRDF(directional_light, view, frag_in.normal, out_color, roughness, vec3(0.3));

    out_color = vec3(texture(tex_1, frag_in.uv));

    color = vec4(out_color, 1.0f);
}