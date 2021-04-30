/*******************************************************************************************
*
*   raylib [others] example - quads instanced
*
********************************************************************************************/

#include "glad.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

// Get the viewport rectangle from the current opengl context.
Rectangle GetViewport()
{
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    Rectangle result = { view[0], view[1], view[2], view[3] };
    return result;
}

// Convert screen coordinates to normalized device coordinates.
Vector2 ScreenToNormalized(Vector2 screen)
{
    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f * screen.x) / (float)GetScreenWidth() - 1.0f;
    float y = 1.0f - (2.0f * screen.y) / (float)GetScreenHeight();

    // Store values in a vector
    Vector2 deviceCoords = { x, y };

    return deviceCoords;
}

// Convert normalized device coordinates to screen coordinates.
Vector2 NormalizedToScreen(Vector2 deviceCoords)
{
    Rectangle view = GetViewport();
    Vector2 screen = { 0 };

    screen.x = ((view.width * 0.5f) * deviceCoords.x) + ((GetScreenWidth() * 0.5f) + view.x);
    screen.y = ((view.height * 0.5f) * deviceCoords.y) + ((GetScreenHeight() * 0.5f) + view.y);

    return screen;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib instancing example - quads_instanced");

    Shader shader = LoadShader("resources/shaders/quads_instanced.vs", "resources/shaders/quads_instanced.fs");

    // Generate a list of 100 quad locations/translation-vectors
    // These are in normalized device coordinates.
    // You need to convert them if using raylib functions to draw.
    //--------------------------------------------------------------------------------------
    Vector2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            Vector2 translation = { 0 };
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index] = translation;
            index += 1;
        }
    }

    // Configure instanced buffer
    // -------------------------
    RenderBatch batch = rlLoadRenderBatch(1, 8192);
    batch.instances = 100;

    int loc = GetShaderLocation(shader, "offsets");
    SetShaderValueV(shader, loc, &translations, SHADER_UNIFORM_VEC2, 1);

    Vector2 size = NormalizedToScreen((Vector2) { 0.10f, 0.10f });
    size.x -= GetScreenWidth() / 2;
    size.y -= GetScreenHeight() / 2;

    bool drawInstanced = false;

    const int instanceCount = 100;

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
        //--------------------------------------------------------------------------------------

        // Draw
        //--------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground((Color) { 26, 26, 26, 255 });

        if (drawInstanced)
        {
            // Draw instanced quads
            BeginShaderMode(shader);
            rlSetRenderBatchActive(&batch);
            // rlViewport(0, 0, 1, 1);

            Vector2 position = NormalizedToScreen((Vector2) { translations[0].x, translations[0].y });
            DrawRectangleGradientEx(
                (Rectangle) { position.x, position.y, size.x, size.y },
                (Color) { 255, 0, 0, 255 },
                (Color) { 0, 0, 255, 255 },
                (Color) { 0, 255, 0, 255 },
                (Color) { 0, 255, 255, 255 }
            );

            rlDrawRenderBatchActive();

            // rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
            rlSetRenderBatchActive(NULL);
            EndShaderMode();
        }
        else
        {
            // Draw each quad in a loop(convert normalized coords)
            for (int i = 0; i < instanceCount; i += 1)
            {
                Vector2 position = NormalizedToScreen((Vector2) { translations[i].x, translations[i].y });
                DrawRectangleGradientEx(
                    (Rectangle) { position.x, position.y, size.x, size.y },
                    (Color) { 255, 0, 0, 255 },
                    (Color) { 0, 0, 255, 255 },
                    (Color) { 0, 255, 0, 255 },
                    (Color) { 0, 255, 255, 255 }
                );
            }
        }

        DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(FormatText("instanced: %i", drawInstanced), 550, 10, 20, MAROON);

        DrawFPS(10, 10);

        EndDrawing();
        //--------------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader); // Unload shader

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
