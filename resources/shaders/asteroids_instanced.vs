#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 8) in mat4 aInstanceMatrix;

out vec2 TexCoords;

uniform mat4 mvp;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    mat4 m = model * aInstanceMatrix;
    gl_Position = (projection * view * m) * vec4(aPos, 1.0f);
}
