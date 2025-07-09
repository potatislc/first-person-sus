#shader vertex
#version 330 core
// #include Material.glsh

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_normal;

out vec3 v_vertexColor;
out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_fragPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
    v_vertexColor = a_color;
    v_texCoord = a_uv;
    v_normal = a_normal;// mat3(transpose(inverse(model))) * aNormal; For model matrices with non uniformed scale
    v_fragPos = vec3(u_model * vec4(a_pos, 1.0));
}

#shader fragment
#version 330 core

in vec3 v_vertexColor;
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_fragPos;

out vec4 fragColor;

uniform sampler2D u_texture1;
uniform vec3 u_lightColor;
uniform float u_ambientLightStrength;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform float u_specularity;

void main() {
    vec4 texColor = texture(u_texture1, v_texCoord);

    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);

    vec3 ambient = u_lightColor * u_ambientLightStrength;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = u_specularity * spec * u_lightColor;

    vec4 result = texColor * vec4(v_vertexColor * (ambient + diffuse + specular), 1.0);
    fragColor = result;
}

