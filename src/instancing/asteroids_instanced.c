/*******************************************************************************************
 *
 *   raylib [models] example - asteroids instanced
 *
 *   My version of the asteroids example from learnopengl at
 *   https://learnopengl.com/Advanced-OpenGL/Instancing
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "camera_first_person.h"

// Required for: calloc(), free()
#include <stdlib.h>

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - asteroids instanced");

    Shader rockShader = LoadShader("resources/shaders/asteroids_instanced.vs", "resources/shaders/asteroids_instanced.fs");
    rockShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(rockShader, "mvp");
    rockShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(rockShader, "instance");
    rockShader.locs[SHADER_LOC_MATRIX_VIEW] = GetShaderLocation(rockShader, "view");
    rockShader.locs[SHADER_LOC_MATRIX_PROJECTION] = GetShaderLocation(rockShader, "projection");

    // Load models
    Model planet = LoadModel("resources/objects/planet/planet.obj");
    Model rock = LoadModel("resources/objects/rock/rock.obj");

    // Generate a large list of semi-random model transformation matrices
    //--------------------------------------------------------------------------------------
    unsigned int asteroidCount = 50000;
    Matrix* modelMatrices = (Matrix*)RL_CALLOC(asteroidCount, sizeof(Matrix));

    // Initialize random seed
    srand(GetTime());

    float radius = 150.0;
    float offset = 30.0f;

    for (int i = 0; i < asteroidCount; i += 1)
    {
        Matrix model = MatrixIdentity();

        // 1. Translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)asteroidCount * 360.0f;

        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

        // Keep height of rock field smaller compared to width of x and z
        float y = displacement * 0.5f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

        float z = cos(angle) * radius + displacement;
        Matrix matTranslation = MatrixTranslate(x, y, z);

        // 2. Scale: Scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        Matrix matScale = MatrixScale(scale, scale, scale);

        // 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        Matrix matRotation = MatrixRotate((Vector3) { 0.4f, 0.6f, 0.8f }, rotAngle);

        model = MatrixMultiply(model, matTranslation);
        model = MatrixMultiply(matRotation, model);
        model = MatrixMultiply(matScale, model);

        // 4. Now add to list of matrices
        modelMatrices[i] = model;
    }

    bool drawInstanced = true;
    bool paused = false;

    // Define the camera to look into our 3d world
    CameraFP camera = LoadCameraFP();
    camera.view.position = (Vector3) { 0.0f, 14.0f, 240.0f };

    Vector2 mousePosition = GetMousePosition();
    Vector2 mouseLastPosition = mousePosition;

    DisableCursor();

    float angle = 0.0f;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float dt = GetFrameTime();
        if (!paused)
        {
            // Track mouse movement
            mousePosition = GetMousePosition();
            Vector2 mouseDelta = Vector2Subtract(mousePosition, mouseLastPosition);
            mouseLastPosition = mousePosition;

            UpdateCameraCustom(&camera, mouseDelta, dt);
            angle += 0.3f * dt;
        }

        if (IsKeyPressed(KEY_R))
        {
            camera.view.position = (Vector3) { 0.0f, 14.0f, 240.0f };
            camera.view.target = (Vector3) { 0.0f, 0.0f, 0.0f };
            camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
        }

        if (IsKeyPressed(KEY_F3))
        {
            if (paused)
            {
                paused = false;
                DisableCursor();
            }
            else
            {
                paused = true;
                EnableCursor();
            }
        }

        // Turn instancing on/off
        if (IsKeyPressed(KEY_ONE))
            drawInstanced = false;
        if (IsKeyPressed(KEY_TWO))
            drawInstanced = true;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground((Color) { 26, 26, 26, 255 });

        BeginMode3D(camera.view);

        rlPushMatrix();
        rlRotatef(angle, 0, 1, 0);

        Vector3 axis = { 0.0f, 0.0f, 1.0f };
        Vector3 scale = { 5.0f, 5.0f, 5.0f };
        DrawModelEx(planet, Vector3Zero(), axis, angle, scale, WHITE);

        // Draw all asteroids at once
        // 1 draw call is made per mesh in the model
        if (drawInstanced)
        {
            rock.transform = MatrixIdentity();
            rock.materials[0].shader = rockShader;
            DrawMeshInstanced(rock.meshes[0], rock.materials[0], modelMatrices, asteroidCount);
        }
        // Draw each asteroid one at a time
        else
        {
            rock.materials[0].shader = rlGetShaderDefault();
            for (int i = 0; i < asteroidCount; i++)
            {
                rock.transform = modelMatrices[i];
                DrawModel(rock, Vector3Zero(), 1.0f, WHITE);
            }
        }

        rlPopMatrix();

        EndMode3D();

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("asteroids: %i", asteroidCount), 120, 10, 20, GREEN);
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    RL_FREE(modelMatrices); // Unload modelMatrices data array

    UnloadModel(planet); // Unload planet model
    UnloadModel(rock);   // Unload rock model
    UnloadShader(rockShader);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
