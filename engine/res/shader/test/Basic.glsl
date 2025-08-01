#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_texCoord;

uniform mat4 u_Mvp;

void main() {
    gl_Position = u_Mvp * position;
    v_texCoord = texCoord;
}

#shader fragment
#version 330 core

in vec2 v_texCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

out vec4 FragColor;// Declare an output variable

void main() {
    vec4 texColor = texture(u_Texture, v_texCoord);
    FragColor = texColor * (u_Color * 4);// Use the output variable
}
