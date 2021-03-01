/*******************************************************************************************
*
*   raylib [others] example - 2d instancing testbed
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

typedef enum InstanceCommand {
    INSTANCE_LINE,
    INSTANCE_TRIANGLE,
    INSTANCE_TRIANGLE_LINE,
    INSTANCE_RECTANGLE,
    INSTANCE_RECTANGLE_LINE,
    INSTANCE_CIRCLE,
    INSTANCE_CIRCLE_LINE,
    INSTANCE_TEXT,
    INSTANCE_TEXTURE,
    MAX_COMMANDS
} InstanceCommand;

void DrawCommand(InstanceCommand command, Texture2D texture, Color color)
{
    // Rectangle frameRec = { 0.0f, 0.0f, (float)texture.width / 6, (float)texture.height };

    switch (command)
    {
        case INSTANCE_LINE:
            DrawLine(0, 0, 50, 0, color);
            break;
        case INSTANCE_TRIANGLE:
            DrawTriangle((Vector2) { 0.0f, 0.0f }, (Vector2) { 50.0f, 0.0f }, (Vector2) { 0.0f, 50.0f }, color);
            break;
        case INSTANCE_TRIANGLE_LINE:
            DrawTriangleLines((Vector2) { 0.0f, 0.0f }, (Vector2) { 50.0f, 0.0f }, (Vector2) { 0.0f, 50.0f }, color);
            break;
        case INSTANCE_RECTANGLE:
            DrawRectangle(0, 0, 50, 50, color);
            break;
        case INSTANCE_RECTANGLE_LINE:
            // DrawRectangleLines(0, 0, 50, 50, color);
            DrawRectangleLinesEx((Rectangle) { 0, 0, 50, 50 }, 3, color);
            break;
        case INSTANCE_CIRCLE:
            DrawCircle(25, 25, 25, color);
            break;
        case INSTANCE_CIRCLE_LINE:
            DrawCircleLines(25, 25, 25, color);
            break;
        case INSTANCE_TEXT:
            DrawText("Text!", 0, 0, 20, color);
            break;
        case INSTANCE_TEXTURE:
            DrawTexture(texture, 0, 0, WHITE);
            // DrawTexturePro(texture, frameRec, (Rectangle) { 0, 0, 50, 50 }, Vector2Zero(), 0, WHITE);
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

    Texture2D texture = LoadTexture("resources/images/wabbit_alpha.png");
    Shader instanceShader = LoadShader("resources/shaders/test_instanced.vs", "resources/shaders/test_instanced.fs");

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, FILTER_BILINEAR);

    // Generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int instanceCount = 200;
    Matrix* modelMatrices = (Matrix*)RL_CALLOC(instanceCount, sizeof(Matrix));

    bool drawInstanced = true;

    // 2D camera mode
    Camera2D camera = { 0 };
    camera.target = (Vector2) { 0, 0 };
    camera.offset = (Vector2) { 0, 50 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    int command = INSTANCE_LINE;

    EnableCursor();

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    glDisable(GL_CULL_FACE);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_F2))
            drawInstanced = !drawInstanced;

        if (IsKeyPressed(KEY_ONE))
            instanceCount -= 10;
        if (IsKeyPressed(KEY_TWO))
            instanceCount += 10;

        if (IsKeyPressed(KEY_LEFT))
            command -= 1;
        if (IsKeyPressed(KEY_RIGHT))
            command += 1;

        if (command < 0)
            command = MAX_COMMANDS - 1;
        if (command > MAX_COMMANDS - 1)
            command = 0;

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
            BeginModeInstanced(instanceCount);

            BeginShaderMode(instanceShader);
            DrawCommand(command, texture, BLUE);
            EndShaderMode();

            EndModeInstanced();
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

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    RL_FREE(modelMatrices); // Unload modelMatrices data array
    UnloadShader(instanceShader);
    UnloadRenderTexture(target);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
