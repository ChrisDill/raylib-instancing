/*******************************************************************************************
*
*   raylib [others] example - 3d instancing testbed
*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include "camera_first_person.h"

// Required for: malloc(), free()
#include <stdlib.h>

typedef enum DrawCommandType {
    DRAW_LINE_3D,
    DRAW_TRIANGLE_3D,
    DRAW_CUBE,
    DRAW_CUBE_WIRES,
    DRAW_SPHERE,
    DRAW_SPHERE_WIRES,
    DRAW_CYLINDER,
    DRAW_CYLINDER_WIRES,
    MAX_DRAW_TYPES
} DrawCommandType;

static const char *drawTypeText[] = {
    "DRAW_LINE_3D",
    "DRAW_TRIANGLE_3D",
    "DRAW_CUBE",
    "DRAW_CUBE_WIRES",
    "DRAW_SPHERE",
    "DRAW_SPHERE_WIRES",
    "DRAW_CYLINDER_WIRES",
    "DRAW_CYLINDER_WIRES",
    "MAX_DRAW_TYPES"
};

void DrawCommand(int command, Color color)
{
    switch (command)
    {
        case DRAW_LINE_3D:
            DrawLine3D(Vector3Zero(), (Vector3){ 0, 0, 10 }, color);
            break;
        case DRAW_TRIANGLE_3D:
            DrawTriangle3D((Vector3){ 0, 0, 0 }, (Vector3){ 10, 0, 0 }, (Vector3){ 0, 10, 0 }, color);
            break;
        case DRAW_CUBE:
            DrawCube((Vector3) { 0, 5.0f, 0 }, 10.0f, 10.0f, 10.0f, color);
            break;
        case DRAW_CUBE_WIRES:
            DrawCubeWires((Vector3) { 0, 5.0f, 0 }, 10.0f, 10.0f, 10.0f, color);
            break;
        case DRAW_SPHERE:
            DrawSphere((Vector3){ 0.0f, 5.0f, 0.0f }, 5.0f, color);
            break;
        case DRAW_SPHERE_WIRES:
            DrawSphereWires((Vector3){ 0.0f, 5.0f, 0.0f }, 5.0f, 16, 16, color);
            break;
        case DRAW_CYLINDER:
            DrawCylinder((Vector3){ 0.0f, 0.0f, 0.0f }, 5.0f, 5.0f, 10.0f, 16, color);
            break;
        case DRAW_CYLINDER_WIRES:
            DrawCylinderWires((Vector3){ 0.0f, 0.0f, 0.0f }, 5.0f, 5.0f, 10.0f, 16, color);
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

    Shader instancedShader = LoadShader("resources/shaders/shapes_instanced_3d.vs", NULL);

    // Configure instanced buffer
    // -------------------------
    rlRenderBatch batch = rlLoadRenderBatch(1, 512);
    batch.instances = 300;

    bool drawInstanced = true;
    int command = DRAW_LINE_3D;

    // Define the camera to look into our 3d world
    CameraFP camera = LoadCameraFP((Vector3){ 0.0f, 14.0f, 240.0f });
    camera.view.position = (Vector3){ 0.0f, 30.0f, 200.0f };

    Vector2 mousePosition = GetMousePosition();
    Vector2 mouseLastPosition = mousePosition;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    DisableCursor();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePosition = GetMousePosition();
        Vector2 mouseDelta = Vector2Subtract(mousePosition, mouseLastPosition);
        mouseLastPosition = mousePosition;

        if (camera.view.projection == CAMERA_CUSTOM)
        {
            UpdateCameraCustom(&camera, mouseDelta, GetFrameTime());
        }

        // Turn instancing on/off
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;

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

        DrawGrid(20, 50.0f);

        if (drawInstanced)
        {
            rlSetRenderBatchActive(&batch);

            BeginShaderMode(instancedShader);
            DrawCommand(command, BLUE);
            EndShaderMode();

            rlDrawRenderBatch(&batch);
            rlSetRenderBatchActive(NULL);
        }
        else
        {
            DrawCommand(command, BLUE);
        }

        EndMode3D();

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
    rlUnloadRenderBatch(batch);
    UnloadShader(instancedShader);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
