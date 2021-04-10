/*******************************************************************************************
*
*   raylib [others] example - 2d instancing testbed
*
********************************************************************************************/

#include "glad.h"
#include "raylib.h"
#include "raylib/src/rlgl.h"

// Required for: malloc(), free()
#include <stdlib.h>

typedef enum DrawCommand {
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
} DrawCommand;

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

void DrawCommand(int command, Texture2D texture, Color color)
{
    switch (command)
    {
        case DRAW_LINE:
            DrawLine(0, 0, 50, 0, color);
            break;
        case DRAW_TRIANGLE:
            DrawTriangle((Vector2) { 50.0f, 0.0f }, (Vector2) { 0.0f, 0.0f }, (Vector2) { 0.0f, 50.0f }, color);
            break;
        case DRAW_TRIANGLE_LINE:
            DrawTriangleLines((Vector2) { 50.0f, 0.0f }, (Vector2) { 0.0f, 0.0f }, (Vector2) { 0.0f, 50.0f }, color);
            break;
        case DRAW_RECTANGLE:
            DrawRectangle(0, 0, 50, 50, color);
            break;
        case DRAW_RECTANGLE_LINE:
            DrawRectangleLinesEx((Rectangle) { 0, 0, 50, 50 }, 3, color);
            break;
        case DRAW_CIRCLE:
            DrawCircle(25, 25, 25, color);
            break;
        case DRAW_CIRCLE_LINE:
            DrawCircleLines(25, 25, 25, color);
            break;
        case DRAW_TEXT:
            DrawText("Text!", 0, 0, 20, color);
            break;
        case DRAW_TEXTURE:
            DrawTexture(texture, 0, 0, WHITE);
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

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [others] example - 2d instancing testbed");

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, FILTER_BILINEAR);

    Texture2D texture = LoadTexture("resources/images/wabbit_alpha.png");
    Shader instanceShader = LoadShader("resources/shaders/test_instanced.vs", "resources/shaders/test_instanced.fs");

    // Configure instanced buffer
    // -------------------------
    RenderBatch batch = rlLoadRenderBatch(1, 8192);
    batch.instances = 200;
    unsigned int instanceCount = 200;

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
        if (IsKeyPressed(KEY_F2))
            drawInstanced = !drawInstanced;

        // Adjust instance count
        if (IsKeyPressed(KEY_ONE))
            instanceCount -= 10;
        if (IsKeyPressed(KEY_TWO))
            instanceCount += 10;
        batch.instances = instanceCount;

        // Switch between draw commands
        if (IsKeyPressed(KEY_LEFT))
            command -= 1;
        if (IsKeyPressed(KEY_RIGHT))
            command += 1;

        if (command >= MAX_DRAW_TYPES)
            command = 0;
        if (command < 0)
            command = MAX_DRAW_TYPES - 1;

        // Camera controls
        if (IsKeyDown(KEY_W))
            camera.target.y -= (int)(300.0f * GetFrameTime());
        if (IsKeyDown(KEY_S))
            camera.target.y += (int)(300.0f * GetFrameTime());
        if (IsKeyDown(KEY_A))
            camera.target.x -= (int)(300.0f * GetFrameTime());
        if (IsKeyDown(KEY_D))
            camera.target.x += (int)(300.0f * GetFrameTime());

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
            instanceCount = 20;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        if (drawInstanced)
        {
            BeginShaderMode(instanceShader);

            rlSetRenderBatchActive(&batch);
            DrawCommand(command, texture, BLUE);
            rlDrawRenderBatchActive();
            rlSetRenderBatchActive(NULL);

            EndShaderMode();
        }
        else
        {
            DrawCommand(command, texture, BLUE);
        }

        EndMode2D();

        EndTextureMode();

        // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
        DrawTextureRec(target.texture, (Rectangle) { 0, 0, target.texture.width, -target.texture.height }, (Vector2) { 0, 0 }, WHITE);

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("instanceCount: %i", instanceCount), 120, 10, 20, GREEN);
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawText(FormatText("%s", drawTypeText[command]), 10, GetScreenHeight() - 20, 14, BLACK);

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
