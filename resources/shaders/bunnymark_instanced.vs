#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

layout (location = 5) in vec2 bunnyPosition;
layout (location = 6) in vec4 bunnyColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    // fragColor = vertexColor;
    fragColor = bunnyColor;

    vec3 position = vertexPosition + vec3(bunnyPosition, 0.0);

    // Calculate final vertex position
    gl_Position = mvp*vec4(position, 1.0);
}
