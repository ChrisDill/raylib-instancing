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

    // https://stackoverflow.com/questions/11316490/convert-a-1d-array-index-to-a-3d-array-index
    int sizeX = 10;
    int sizeY = 10;
    int sizeZ = 10;
    int x = gl_InstanceID % sizeZ;
    int z = (gl_InstanceID / sizeZ) % sizeY;
    int y = gl_InstanceID / (sizeY * sizeZ);

    position.x += x * 50.0f;
    position.y += y * 50.0f;
    position.z += z * 50.0f;

    position -= vec3(sizeX * 50.0f, 0, sizeZ * 50.0f) / 2;

    gl_Position = mvp * vec4(position, 1.0);
}
