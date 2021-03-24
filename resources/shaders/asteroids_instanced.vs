#version 330 core

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;

layout (location = 12) in mat4 instance;

// Input uniform values
uniform mat4 mvp;
uniform mat4 projection;
uniform mat4 view;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;

    // Calculate final vertex position
    mat4 mvpi = mvp * instance;
    gl_Position = mvpi * vec4(vertexPosition, 1.0);

    // mat4 m = instance;
    // gl_Position = (projection * view) * vec4(vertexPosition, 1.0f);
}

/*
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
*/
