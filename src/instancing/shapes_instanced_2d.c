/*******************************************************************************************
*
*   raylib [others] example - 2d instancing testbed
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

// Required for: malloc(), free()
#include <stdlib.h>

typedef enum DrawCommandType {
    DRAW_LINE,
    DRAW_TRIANGLE,
    DRAW_TRIANGLE_LINE,
    DRAW_RECTANGLE,
    DRAW_RECTANGLE_LINE,
    DRAW_CIRCLE,
    DRAW_CIRCLE_LINE,
    DRAW_TEXT,
    DRAW_TEXTURE,
    MAX_DRAW_TYPES
} DrawCommandType;

static const char *drawTypeText[] = {
    "DRAW_LINE",
    "DRAW_TRIANGLE",
    "DRAW_TRIANGLE_LINE",
    "DRAW_RECTANGLE",
    "DRAW_RECTANGLE_LINE",
    "DRAW_CIRCLE",
    "DRAW_CIRCLE_LINE",
    "DRAW_TEXT",
    "DRAW_TEXTURE",
};

void DrawCommand(int command, Vector2 position, Texture2D texture, Color color)
{
    int x = position.x;
    int y = position.y;

    switch (command)
    {
        case DRAW_LINE:
            DrawLine(x, y, x + 50, y + 0, color);
            break;
        case DRAW_TRIANGLE:
            DrawTriangle((Vector2) { x + 50.0f, y }, (Vector2) { x, y }, (Vector2) { x, y + 50.0f }, color);
            break;
        case DRAW_TRIANGLE_LINE:
            DrawTriangleLines((Vector2) { x + 50.0f, y }, (Vector2) { x, y }, (Vector2) { x, y + 50.0f }, color);
            break;
        case DRAW_RECTANGLE:
            DrawRectangle(x, y, 50, 50, color);
            break;
        case DRAW_RECTANGLE_LINE:
            DrawRectangleLinesEx((Rectangle) { x, y, 50, 50 }, 3, color);
            break;
        case DRAW_CIRCLE:
            DrawCircle(x + 25, y + 25, 25, color);
            break;
        case DRAW_CIRCLE_LINE:
            DrawCircleLines(x + 25, y + 25, 25, color);
            break;
        case DRAW_TEXT:
            DrawText("Text!", x, y, 20, color);
            break;
        case DRAW_TEXTURE:
            DrawTexture(texture, x, y, WHITE);
            break;
        default:
            break;
    }
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [others] example - 2d instancing testbed");

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    Texture2D texture = LoadTexture("resources/images/wabbit_alpha.png");
    Shader instanceShader = LoadShader("resources/shaders/shapes_instanced_2d.vs", NULL);

    // Configure instanced buffer
    // -------------------------
    rlRenderBatch batch = rlLoadRenderBatch(1, 36);
    batch.instances = 300;

    bool drawInstanced = false;

    // 2D camera mode
    Camera2D camera = { 0 };
    camera.target = (Vector2) { 0, 0 };
    camera.offset = (Vector2) { 0, 50 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    int command = DRAW_LINE;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float dt = GetFrameTime();
        if (IsKeyDown(KEY_W))
            camera.target.y -= (int)(300.0f * dt);
        if (IsKeyDown(KEY_S))
            camera.target.y += (int)(300.0f * dt);
        if (IsKeyDown(KEY_A))
            camera.target.x -= (int)(300.0f * dt);
        if (IsKeyDown(KEY_D))
            camera.target.x += (int)(300.0f * dt);

        // Turn instancing on/off
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

        // Switch between draw commands
        if (IsKeyPressed(KEY_LEFT))
            command -= 1;
        if (IsKeyPressed(KEY_RIGHT))
            command += 1;

        if (command >= MAX_DRAW_TYPES)
            command = 0;
        if (command < 0)
            command = MAX_DRAW_TYPES - 1;

        int wheelMove = GetMouseWheelMove();
        if (wheelMove != 0)
        {
            camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
        }

        // Reset test
        if (IsKeyPressed(KEY_R))
        {
            camera.target = Vector2Zero();
            camera.offset = (Vector2) { 0, 50 };
            camera.rotation = 0.0f;
            camera.zoom = 1.0f;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        if (drawInstanced)
        {
            BeginShaderMode(instanceShader);

            rlSetRenderBatchActive(&batch);
            DrawCommand(command, Vector2Zero(), texture, BLUE);
            rlDrawRenderBatchActive();
            rlSetRenderBatchActive(NULL);

            EndShaderMode();
        }
        else
        {
            int width = 30;
            for (int i = 0; i < batch.instances; i++)
            {
                // % is the "modulo operator", the remainder of i / width;
                // where "/" is an integer division
                Vector2 position = { i % width, i / width };
                position.x += position.x * 50.0f;
                position.y += position.y * 50.0f;
                DrawCommand(command, position, texture, BLUE);
            }
        }

        EndMode2D();

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(TextFormat("instanceCount: %i", batch.instances), 120, 10, 20, GREEN);
        DrawText(TextFormat("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawText(TextFormat("%s", drawTypeText[command]), 10, GetScreenHeight() - 20, 14, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(instanceShader);
    UnloadRenderTexture(target);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
