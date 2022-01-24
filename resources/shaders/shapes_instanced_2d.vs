#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate final vertex position
    vec3 position = vertexPosition;

    int width = 30;
    int x = gl_InstanceID % width;    // % is the "modulo operator", the remainder of i / width;
    int y = gl_InstanceID / width;    // where "/" is an integer division

    position.x += x * 50.0f;
    position.y += y * 50.0f;

    gl_Position = mvp * vec4(position, 1.0);
}
