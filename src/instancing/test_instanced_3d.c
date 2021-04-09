/*******************************************************************************************
*
*   raylib [others] example - 3d instancing testbed
*
********************************************************************************************/

#include "glad.h"
#include "raylib.h"
#include "raylib/src/rlgl.h"
#include "camera_first_person.h"

// Required for: malloc(), free()
#include <stdlib.h>

typedef enum InstanceCommand {
    INSTANCE_LINE,
    INSTANCE_TRIANGLE,
    INSTANCE_RECTANGLE,
    INSTANCE_CIRCLE,
    INSTANCE_TEXT,
    INSTANCE_CUBE,
    INSTANCE_SPHERE,
    INSTANCE_MODEL,
    MAX_COMMANDS
} InstanceCommand;

void DrawCommand(InstanceCommand command, Model model, Color color)
{
    switch (command)
    {
        case INSTANCE_LINE:
            DrawLine(0, 0, 100, 0, color);
            break;
        case INSTANCE_TRIANGLE:
            DrawTriangle((Vector2) { 0.0f, 0.0f }, (Vector2) { 100.0f, 0.0f }, (Vector2) { 0.0f, 100.0f }, color);
            break;
        case INSTANCE_RECTANGLE:
            DrawRectangle(0, 0, 100, 100, color);
            break;
        case INSTANCE_CIRCLE:
            DrawCircleLines(0, 0, 30, color);
            break;
        case INSTANCE_TEXT:
            DrawText("Hello World!", 0, 0, 20, color);
            break;
        case INSTANCE_CUBE:
            DrawCube((Vector3) { 0.0f, 0.0f }, 10.0f, 10.0f, 10.0f, color);
            break;
        case INSTANCE_SPHERE:
            break;
        case INSTANCE_MODEL:
            DrawModel(model, Vector3Zero(), 5.0f, color);
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
    int command = INSTANCE_TRIANGLE;

    // Define the camera to look into our 3d world
    CameraFP camera = LoadCameraFP((Vector3) { 0.0f, 14.0f, 240.0f });
    camera.view.position = (Vector3) { 0.0f, 30.0f, 100.0f };

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
            // Mouse movement
            mousePosition = GetMousePosition();
            Vector2 mouseDelta = Vector2Subtract(mousePosition, mouseLastPosition);
            mouseLastPosition = mousePosition;

            if (camera.view.projection == CAMERA_CUSTOM)
            {
                UpdateCameraCustom(&camera, mouseDelta, GetFrameTime());
            }
        }

        if (IsKeyPressed(KEY_ONE))
        {
            drawInstanced = false;
        }
        if (IsKeyPressed(KEY_TWO))
        {
            drawInstanced = true;
        }

        if (IsKeyPressed(KEY_W))
        {
            instanceCount += 1;
        }
        if (IsKeyPressed(KEY_S))
        {
            instanceCount -= 1;
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            command -= 1;
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            command += 1;
        }

        if (command < 0)
        {
            command = MAX_COMMANDS - 1;
        }
        if (command > MAX_COMMANDS - 1)
        {
            command = 0;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera.view);

        DrawGrid(20, 30.0f);

        if (drawInstanced)
        {
            batch.instances = instanceCount;
            rlSetRenderBatchActive(&batch);

            BeginShaderMode(instancedShader);
            DrawCommand(command, model, WHITE);
            EndShaderMode();

            rlSetRenderBatchActive(&batch);
        }
        else
        {
            DrawCommand(command, model, WHITE);
        }

        EndMode3D();

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("models: %i", instanceCount), 120, 10, 20, GREEN);
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

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
