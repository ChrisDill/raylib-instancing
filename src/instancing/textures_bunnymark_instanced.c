/*******************************************************************************************
*
*   raylib [textures] example - Bunnymark instanced
*
*   Based on the the existing textures_bunnymark example.
*
*   This example has been created using raylib 3.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2020 Chris Dill
*
********************************************************************************************/

#include "glad.h"
#include "raylib.h"
#include "rlgl.h"
#include <stddef.h>
#include <stdlib.h> // Required for: malloc(), free()

#define MAX_BUNNIES 500000 // 500K bunnies limit with instancing!

// This is the maximum amount of elements (quads) per batch
// NOTE: This value is defined in [rlgl] module and can be changed there
#define MAX_BATCH_ELEMENTS 8192

typedef struct Bunny {
    Vector2 position;
    Vector2 speed;
    Color color;
} Bunny;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [textures] example - bunnymark instanced");

    // Load bunny texture
    Texture2D texBunny = LoadTexture("resources/images/wabbit_alpha.png");

    Shader shader = LoadShader("resources/shaders/bunnymark_instanced.vs", "resources/shaders/bunnymark_instanced.fs");

    // Bunnies array
    Bunny* bunnies = (Bunny*)RL_CALLOC(MAX_BUNNIES, sizeof(Bunny));
    int bunniesCount = 0; // Bunnies counter

    // Configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, bunniesCount * sizeof(Bunny), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    rlBindVAO();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // Shader attribute locations
    int positionAttrib = glGetAttribLocation(shader.id, "bunnyPosition");
    int colorAttrib = glGetAttribLocation(shader.id, "bunnyColor");

    // Bunny positions
    // 2 x float = 2 x GL_FLOAT
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Bunny), (void*)0);

    // Bunny colors
    // 4 x unsigned char = 4 x GL_UNSIGNED_BYTE
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Bunny), (void*)offsetof(Bunny, color));

    // Make attributes instanced
    glVertexAttribDivisor(positionAttrib, 1);
    glVertexAttribDivisor(colorAttrib, 1);

    glBindVertexArray(0);

    bool drawInstanced = false;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            // Create more bunnies
            for (int i = 0; i < 100; i++)
            {
                if (bunniesCount < MAX_BUNNIES)
                {
                    bunnies[bunniesCount].position = GetMousePosition();
                    bunnies[bunniesCount].speed.x = (float)GetRandomValue(-250, 250) / 60.0f;
                    bunnies[bunniesCount].speed.y = (float)GetRandomValue(-250, 250) / 60.0f;
                    bunnies[bunniesCount].color = (Color) { GetRandomValue(50, 240),
                        GetRandomValue(80, 240),
                        GetRandomValue(100, 240), 255 };
                    bunniesCount++;
                }
            }
        }

        // Update bunnies
        for (int i = 0; i < bunniesCount; i++)
        {
            bunnies[i].position.x += bunnies[i].speed.x;
            bunnies[i].position.y += bunnies[i].speed.y;

            if (((bunnies[i].position.x + texBunny.width / 2) > GetScreenWidth()) || ((bunnies[i].position.x + texBunny.width / 2) < 0))
                bunnies[i].speed.x *= -1;
            if (((bunnies[i].position.y + texBunny.height / 2) > GetScreenHeight()) || ((bunnies[i].position.y + texBunny.height / 2 - 40) < 0))
                bunnies[i].speed.y *= -1;
        }

        // Re-upload bunnies array every frame to apply movement
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, bunniesCount * sizeof(Bunny), &bunnies[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (drawInstanced)
        {
            BeginShaderMode(shader);
            BeginModeInstanced(bunniesCount);
            DrawTexture(texBunny, 0, 0, WHITE);
            rlglDraw();
            EndModeInstanced();
            EndShaderMode();
        }
        else
        {
            for (int i = 0; i < bunniesCount; i++)
            {
                // NOTE: When internal batch buffer limit is reached (MAX_BATCH_ELEMENTS),
                // a draw call is launched and buffer starts being filled again;
                // before issuing a draw call, updated vertex data from internal CPU buffer is send to GPU...
                // Process of sending data is costly and it could happen that GPU data has not been completely
                // processed for drawing while new data is tried to be sent (updating current in-use buffers)
                // it could generates a stall and consequently a frame drop, limiting the number of drawn bunnies
                DrawTexture(texBunny, bunnies[i].position.x, bunnies[i].position.y, bunnies[i].color);
            }
        }

        DrawRectangle(0, 0, GetScreenWidth(), 40, BLACK);
        DrawText(FormatText("bunnies: %i", bunniesCount), 120, 10, 20, GREEN);

        if (!drawInstanced)
        {
            DrawText(FormatText("batched draw calls: %i", 1 + bunniesCount / MAX_BATCH_ELEMENTS), 300, 10, 20, MAROON);
        }
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    RL_FREE(bunnies); // Unload bunnies data array

    UnloadTexture(texBunny); // Unload bunny texture
    UnloadShader(shader);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
