/*******************************************************************************************
*
*   raylib [models] example - particles instanced
*
*   This example has been created using raylib 3.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2020 Chris Dill
*
********************************************************************************************/

#include "glad.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdlib.h> // Required for: malloc(), free()

#define MAX_PARTICLES 100000

typedef struct Particle {
    Vector2 position;
    Vector2 speed;
    Color color;
} Particle;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [others] example - particles instanced");

    Texture2D texParticle = LoadTexture("resources/wabbit_alpha.png");
    Shader shader = LoadShader("resources/shaders/asteroids_instanced.vs", "resources/shaders/asteroids_instanced.fs");

    // Particles array
    Particle* particles = (Particle*)RL_CALLOC(MAX_PARTICLES, sizeof(Particle));

    // Particles counter
    int particleCount = 0;

    // Configure instanced array
    //--------------------------------------------------------------------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(Particle), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    rlBindVAO();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // Shader attribute locations.
    int positionAttrib = glGetAttribLocation(shader.id, "ParticlePosition");
    int colorAttrib = glGetAttribLocation(shader.id, "ParticleColor");

    // Particle positions.
    // 2 x float = 2 x GL_FLOAT
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);

    // Particle colors.
    // 4 x unsigned char = 4 x GL_UNSIGNED_BYTE
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Particle), (void*)offsetof(Particle, color));

    // Make attributes instanced.
    glVertexAttribDivisor(positionAttrib, 1);
    glVertexAttribDivisor(colorAttrib, 1);

    glBindVertexArray(0);

    bool drawInstanced = false;

    EnableCursor();

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second.
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key.
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            // Create more particles.
            for (int i = 0; i < 100; i++)
            {
                if (particleCount < MAX_PARTICLES)
                {
                    particles[particleCount].position = GetMousePosition();
                    particles[particleCount].speed.x = 0.0f;
                    particles[particleCount].speed.y = GetRandomValue(0, 250) / 60.0f;
                    particles[particleCount].color = (Color) { GetRandomValue(50, 240),
                        GetRandomValue(80, 240),
                        GetRandomValue(100, 240), 255 };
                    particleCount += 1;
                }
            }
        }

        // Update particles
        for (int i = 0; i < particleCount; i++)
        {
            particles[i].position.x += particles[i].speed.x;
            particles[i].position.y += particles[i].speed.y;

            if (((particles[i].position.x + texParticle.width / 2) > GetScreenWidth()) || ((particles[i].position.x + texParticle.width / 2) < 0))
                particles[i].speed.x *= -1;
            if (((particles[i].position.y + texParticle.height / 2) > GetScreenHeight()) || ((particles[i].position.y + texParticle.height / 2 - 40) < 0))
                particles[i].speed.y *= -1;
        }

        // Re-upload particles array every frame to apply movement
        // @TODO look into moving movement into shader code.
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(Particle), &particles[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        if (drawInstanced)
        {
            BeginShaderMode(shader);
            BeginModeInstanced(particleCount);
            DrawTexture(texParticle, 0, 0, WHITE);
            EndModeInstanced();
            EndShaderMode();
        }
        else
        {
            for (int i = 0; i < particleCount; i++)
            {
                // NOTE: When internal batch buffer limit is reached (MAX_BATCH_ELEMENTS),
                // a draw call is launched and buffer starts being filled again;
                // before issuing a draw call, updated vertex data from internal CPU buffer is send to GPU...
                // Process of sending data is costly and it could happen that GPU data has not been completely
                // processed for drawing while new data is tried to be sent (updating current in-use buffers)
                // it could generates a stall and consequently a frame drop, limiting the number of drawn particles
                DrawTexture(texParticle, particles[i].position.x, particles[i].position.y, particles[i].color);
            }
        }

        EndMode3D();

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("particles: %i", particleCount), 120, 10, 20, GREEN);
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    RL_FREE(particles); // Unload modelMatrices data array

    UnloadShader(shader);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
