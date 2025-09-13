#version 330 core
#include ENGINE_RES_PATH/shader/include/Mvp.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_fragPos;

void main() {
    gl_Position = getMvp() * vec4(a_pos, 1.0);
    v_texCoord = a_uv;
    // If you need this then do it on the cpu before renderng the instance: mat3(transpose(inverse(u_model))
    // v_normal = normalize(mat3(transpose(inverse(u_model))) * a_normal);
    v_normal = mat3(u_model) * a_normal;
    v_fragPos = vec3(u_model * vec4(a_pos, 1.0));
}
