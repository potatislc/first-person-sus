#shader vertex
#version 330 core
#include ENGINE_RES_PATH/shader/Mvp.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_normal;

out vec3 v_vertexColor;
out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_fragPos;

void main() {
    gl_Position = getMvp() * vec4(a_pos, 1.0);
    v_vertexColor = a_color;
    v_texCoord = a_uv;
    v_normal = a_normal;// mat3(transpose(inverse(model))) * aNormal; For model matrices with non uniform scale
    v_fragPos = vec3(u_model * vec4(a_pos, 1.0));
}

#shader fragment
#version 330 core
#include ENGINE_RES_PATH/shader/Material.glsl

in vec3 v_vertexColor;
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_fragPos;

out vec4 fragColor;

uniform sampler2D u_texture1;
uniform vec3 u_viewPos;
// uniform Material u_material;
uniform MaterialMap u_matMap;
uniform Light u_light;

void main() {
    vec4 texColor = texture(u_texture1, v_texCoord);

    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec4 result = texColor * vec4(v_vertexColor * phongLighting(sampleMap(u_matMap, v_texCoord), u_light, normal, viewDir, v_fragPos), 1.0);
    fragColor = result;
}

