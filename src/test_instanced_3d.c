/*******************************************************************************************
*
*   raylib [others] example - 3d instancing testbed
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

#include "camera_first_person.c"

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
    unsigned int amount = 20;
    Matrix* modelMatrices = (Matrix*)RL_CALLOC(amount, sizeof(Matrix));

    bool drawInstanced = false;
    bool paused = false;

    int command = INSTANCE_TRIANGLE;

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3) { 0.0f, 30.0f, 100.0f }; // Camera position
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                 // Camera field-of-view Y
    camera.type = CAMERA_PERSPECTIVE;                    // Camera mode type

    SetCameraMode(camera, CAMERA_CUSTOM); // Set a free camera mode
    Load3DCamera();

    Vector2 mousePosition = GetMousePosition();
    Vector2 mouseLastPosition = mousePosition;

    DisableCursor();

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
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

            UpdateCamera(&camera); // Update camera
            if (camera.type == CAMERA_CUSTOM)
            {
                UpdateCameraCustom(&camera, mouseDelta, GetFrameTime());
            }
        }

        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

        if (IsKeyPressed(KEY_W))
            amount += 1;
        if (IsKeyPressed(KEY_S))
            amount -= 1;

        if (IsKeyPressed(KEY_LEFT))
            command -= 1;
        if (IsKeyPressed(KEY_RIGHT))
            command += 1;

        if (command < 0)
            command = MAX_COMMANDS - 1;
        if (command > MAX_COMMANDS - 1)
            command = 0;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);

        DrawGrid(20, 30.0f);

        if (drawInstanced)
        {
            BeginModeInstanced(amount);
            BeginShaderMode(instancedShader);
            DrawCommand(command, model, WHITE);
            EndShaderMode();
            EndModeInstanced();
        }
        else
        {
            DrawCommand(command, model, WHITE);
        }

        EndMode3D();

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("models: %i", amount), 120, 10, 20, GREEN);
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
