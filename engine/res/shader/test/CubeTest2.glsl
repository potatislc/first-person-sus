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
    v_normal = a_normal;// mat3(transpose(inverse(model))) * aNormal; For model matrices with non uniformed scale
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
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform Material u_material;

void main() {
    vec4 texColor = texture(u_texture1, v_texCoord);

    // Normalize normal and calculate lighting directions
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Ambient lighting
    vec3 ambient = u_material.ambientColor * u_lightColor;

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_material.diffuseColor * u_lightColor;

    // Specular lighting
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    vec3 specular = u_material.specularColor * spec * u_lightColor;

    // Combine and apply texture and vertex color
    vec3 lighting = ambient + diffuse + specular;
    vec4 result = texColor * vec4(v_vertexColor * lighting, 1.0);
    fragColor = result;
}

