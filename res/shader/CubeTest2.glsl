#shader vertex
#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_uv;

out vec3 vertexColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    vertexColor = a_color;
    texCoord = a_uv;
}

#shader fragment
#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

out vec4 color;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, texCoord);
    color = texColor * vec4(vertexColor, 1.0);
}

