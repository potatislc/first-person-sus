#shader fragment
#version 330 core

uniform vec3 u_lightColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(u_lightColor, 1.0);
}
