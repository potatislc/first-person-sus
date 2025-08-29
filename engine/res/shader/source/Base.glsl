#shader vertex
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
    v_normal = a_normal;
    v_fragPos = vec3(u_model * vec4(a_pos, 1.0));
}

#shader fragment
#version 330 core
#include ENGINE_RES_PATH/shader/include/Material.glsl

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_fragPos;

out vec4 fragColor;

uniform sampler2D u_texture1;
uniform vec3 u_viewPos;
uniform Material u_material;
uniform DirectionalLight u_light;

void main() {
    vec4 texColor = texture(u_texture1, v_texCoord);

    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec4 result = texColor * vec4(phongLighting(u_material, u_light, normal, viewDir, v_fragPos), 1.0);
    fragColor = result;
}
