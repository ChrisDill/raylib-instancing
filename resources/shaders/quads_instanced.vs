#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform vec2 offsets[100];

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate final vertex position
    vec3 position = vertexPosition;

    vec2 offset = offsets[gl_InstanceID];
    //position.x += gl_InstanceID * 70.0f;

    gl_Position = mvp*vec4(position, 1.0);

    gl_Position.x += offset.x;
    gl_Position.y += offset.y;
}