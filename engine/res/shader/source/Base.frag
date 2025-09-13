#version 330 core
#include ENGINE_RES_PATH/shader/include/Material.glsl

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_fragPos;

uniform sampler2D u_texture1;
uniform vec3 u_viewPos;
uniform Material u_material;
uniform DirectionalLight u_light;

out vec4 fragColor;

void main() {
    vec4 texColor = texture(u_texture1, v_texCoord);

    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewPos - v_fragPos);

    vec4 result = texColor * vec4(phongLighting(u_material, u_light, normal, viewDir, v_fragPos), 1.0);
    fragColor = result;
}
