/*******************************************************************************************
*
*   raylib [textures] example - bunnymark instanced
*
*   Based on the the existing textures_bunnymark example.
*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"

// Required for: malloc(), free()
#include <stdlib.h>

// Required for: offsetof()
#include <stddef.h>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

// 500K bunnies limit with instancing!
#define MAX_BUNNIES 500000

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
    int bunniesCount = 0;

    // Configure instanced buffer
    // -------------------------
    RenderBatch batch = rlLoadRenderBatch(1, 8192);
    batch.instances = bunniesCount;

    rlEnableVertexArray(batch.vertexBuffer[0].vaoId);
    int bufferLength = 400000;
    int buffer = rlLoadVertexBuffer(bunnies, bufferLength * sizeof(Bunny), true);

    // Shader attribute locations
    int positionAttrib = rlGetLocationAttrib(shader.id, "bunnyPosition");
    int colorAttrib = rlGetLocationAttrib(shader.id, "bunnyColor");

    // instanced bunny positions(2 x float = 2 x GL_FLOAT)
    rlEnableVertexAttribute(positionAttrib);
    rlSetVertexAttribute(positionAttrib, 2, RL_FLOAT, false, sizeof(Bunny), (void*)0);
    rlSetVertexAttributeDivisor(positionAttrib, 1);

    // instanced bunny colors(4 x unsigned char = 4 x GL_UNSIGNED_BYTE)
    rlEnableVertexAttribute(colorAttrib);
    rlSetVertexAttribute(colorAttrib, 4, RL_UNSIGNED_BYTE, true, sizeof(Bunny), (void*)offsetof(Bunny, color));
    rlSetVertexAttributeDivisor(colorAttrib, 1);

    rlDisableVertexArray();

    bool drawInstanced = false;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Turn instancing on/off
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

        // Spawn bunnies
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            for (int i = 0; i < 100; i++)
            {
                if (bunniesCount < bufferLength)
                {
                    bunnies[bunniesCount].position = GetMousePosition();
                    bunnies[bunniesCount].speed.x = (float)GetRandomValue(-250, 250) / 60.0f;
                    bunnies[bunniesCount].speed.y = (float)GetRandomValue(-250, 250) / 60.0f;
                    bunnies[bunniesCount].color = (Color) {
                        GetRandomValue(50, 240),
                        GetRandomValue(80, 240),
                        GetRandomValue(100, 240), 255
                    };
                    bunniesCount++;
                    batch.instances = bunniesCount;
                }
            }
        }

        // Update bunnies
        for (int i = 0; i < bunniesCount; i++)
        {
            bunnies[i].position.x += bunnies[i].speed.x;
            bunnies[i].position.y += bunnies[i].speed.y;

            Vector2 center = Vector2Add(bunnies[i].position, (Vector2){ texBunny.width / 2, texBunny.height / 2});
            if (center.x > GetScreenWidth() || center.x < 0)
                bunnies[i].speed.x *= -1;
            if (center.y > GetScreenHeight() || center.y - 40 < 0)
                bunnies[i].speed.y *= -1;
        }

        // Re-upload bunnies array every frame to apply movement
        int length = min(bunniesCount, bufferLength);
        rlUpdateVertexBuffer(buffer, bunnies, length * sizeof(Bunny), 0);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (drawInstanced)
        {
            BeginShaderMode(shader);

            rlSetRenderBatchActive(&batch);
            DrawTexture(texBunny, 0, 0, WHITE);
            rlDrawRenderBatchActive();
            rlSetRenderBatchActive(NULL);

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

    rlUnloadVertexBuffer(buffer);
    rlUnloadRenderBatch(batch);
    UnloadTexture(texBunny); // Unload bunny texture
    UnloadShader(shader);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
