#shader fragment
#version 330 core

in vec3 v_fragPos;
in vec3 v_normal;

uniform vec3 u_lightColor;
uniform vec3 u_lightPos;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;
    fragColor = vec4(diffuse, 1.0);
}