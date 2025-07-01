#shader vertex
#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    vertexColor = a_color;
}

#shader fragment
#version 330 core

in vec3 vertexColor;
out vec4 color;

void main()
{
    color = vec4(vertexColor, 1.0);
}

