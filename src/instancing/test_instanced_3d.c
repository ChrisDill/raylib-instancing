/*******************************************************************************************
*
*   raylib [others] example - 3d instancing testbed
*
********************************************************************************************/

#include "glad.h"
#include "raylib.h"
#include "rlgl.h"
#include "camera_first_person.h"

// Required for: malloc(), free()
#include <stdlib.h>

typedef enum DrawCommandType {
    DRAW_LINE,
    DRAW_TRIANGLE,
    DRAW_RECTANGLE,
    DRAW_CIRCLE,
    DRAW_TEXT,
    DRAW_CUBE,
    DRAW_SPHERE,
    DRAW_MODEL,
    MAX_DRAW_TYPES
} DrawCommandType;

static const char *drawTypeText[] = {
    "DRAW_LINE",
    "DRAW_TRIANGLE",
    "DRAW_RECTANGLE",
    "DRAW_CIRCLE",
    "DRAW_TEXT",
    "DRAW_CUBE",
    "DRAW_SPHERE",
    "DRAW_MODEL",
    "MAX_DRAW_TYPES"
};

void DrawCommand(int command, Model model, Color color)
{
    switch (command)
    {
        case DRAW_LINE:
            DrawLine(0, 0, 100, 0, color);
            break;
        case DRAW_TRIANGLE:
            DrawTriangle((Vector2) { 0.0f, 0.0f }, (Vector2) { 100.0f, 0.0f }, (Vector2) { 0.0f, 100.0f }, color);
            break;
        case DRAW_RECTANGLE:
            DrawRectangle(0, 0, 100, 100, color);
            break;
        case DRAW_CIRCLE:
            DrawCircleLines(0, 0, 30, color);
            break;
        case DRAW_TEXT:
            DrawText("Hello World!", 0, 0, 20, color);
            break;
        case DRAW_CUBE:
            DrawCube((Vector3) { 0.0f, 0.0f }, 10.0f, 10.0f, 10.0f, color);
            break;
        case DRAW_SPHERE:
            break;
        case DRAW_MODEL:
            DrawModel(model, Vector3Zero(), 5.0f, WHITE);
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

    InitWindow(screenWidth, screenHeight, "raylib [others] example - 3d instancing testbed");

    Shader instancedShader = LoadShader("resources/shaders/test_instanced.vs", "resources/shaders/test_instanced.fs");

    Model model = LoadModel("resources/objects/nanosuit/nanosuit.obj");
    model.materials[0].shader = instancedShader;

    // Generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int instanceCount = 20;
    Matrix* modelMatrices = (Matrix*)RL_CALLOC(instanceCount, sizeof(Matrix));

    // Configure instanced buffer
    // -------------------------
    RenderBatch batch = rlLoadRenderBatch(1, 8192);
    batch.instances = 100;

    bool drawInstanced = true;
    bool paused = false;
    int command = DRAW_LINE;

    // Define the camera to look into our 3d world
    CameraFP camera = LoadCameraFP((Vector3) { 0.0f, 14.0f, 240.0f });
    camera.view.position = (Vector3) { 0.0f, 30.0f, 200.0f };

    Vector2 mousePosition = GetMousePosition();
    Vector2 mouseLastPosition = mousePosition;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    DisableCursor();

    glEnable(GL_CULL_FACE_MODE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (!paused)
        {
            mousePosition = GetMousePosition();
            Vector2 mouseDelta = Vector2Subtract(mousePosition, mouseLastPosition);
            mouseLastPosition = mousePosition;

            if (camera.view.projection == CAMERA_CUSTOM)
            {
                UpdateCameraCustom(&camera, mouseDelta, GetFrameTime());
            }
        }

        // Turn instancing on/off
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

        // Adjust instance count
        if (IsKeyPressed(KEY_W))
            instanceCount += 1;
        if (IsKeyPressed(KEY_S))
            instanceCount -= 1;

        // Set draw command
        if (IsKeyPressed(KEY_LEFT))
            command -= 1;
        if (IsKeyPressed(KEY_RIGHT))
            command += 1;

        if (command < 0)
            command = MAX_DRAW_TYPES - 1;
        if (command > MAX_DRAW_TYPES - 1)
            command = 0;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera.view);

        DrawGrid(20, 30.0f);

        if (drawInstanced)
        {
            batch.instances = instanceCount;
            rlSetRenderBatchActive(&batch);

            BeginShaderMode(instancedShader);
            DrawCommand(command, model, BLUE);
            EndShaderMode();

            rlSetRenderBatchActive(&batch);
        }
        else
        {
            DrawCommand(command, model, BLUE);
        }

        EndMode3D();

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("instanceCount: %i", instanceCount), 120, 10, 20, GREEN);
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawText(FormatText("%s", drawTypeText[command]), 10, GetScreenHeight() - 20, 14, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    RL_FREE(modelMatrices); // Unload modelMatrices data array

    UnloadModel(model);
    UnloadShader(instancedShader);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
