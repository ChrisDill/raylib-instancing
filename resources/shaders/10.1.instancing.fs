#version 330 core

in vec3 fColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    finalColor = vec4(fColor, 1.0);
}
