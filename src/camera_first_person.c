#include "raylib.h"
#include "raymath.h"

// Camera constants
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float MovementSpeed = 20.5f;
const float MouseSensitivity = 15.0f;
const Vector3 WorldUp = { 0.0f, 1.0f, 0.0f };

typedef struct CameraFirstPerson {
    float CurrentSpeed;
    Vector3 Front;
    Vector3 Right;
    Vector3 Up;
    float Yaw;
    float Pitch;
    float Zoom;
    bool constrainPitch;
} CameraFirstPerson;

// Camera vars
static float CurrentSpeed = 0;
static Vector3 Front = { 0.0f, 0.0f, -1.0f };
static Vector3 Right = { 1.0f, 0.0f, 0.0f };
static Vector3 Up = { 0.0f, 1.0f, 0.0f };
static float Yaw = 0.0f;
static float Pitch = 0.0f;
static float Zoom = 0.0f;
static bool constrainPitch = true;

void Load3DCamera()
{
    Yaw = YAW;
    Pitch = PITCH;
    Zoom = ZOOM;
    CurrentSpeed = MovementSpeed;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void UpdateCameraVectors(Camera3D* camera)
{
    // Calculate the new Front vector
    Vector3 front = {};
    front.x = cos(DEG2RAD * Yaw) * cos(DEG2RAD * Pitch);
    front.y = sin(DEG2RAD * Pitch);
    front.z = sin(DEG2RAD * Yaw) * cos(DEG2RAD * Pitch);
    Front = Vector3Normalize(front);

    // Also re-calculate the Right and Up vector
    Right = Vector3Normalize(Vector3CrossProduct(Front, WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = Vector3Normalize(Vector3CrossProduct(Right, Front));
}

// Adjust yaw and pitch to look around and contrain pitch(up and down).
void UpdateMouseMovement(Camera3D* camera, int xoffset, int yoffset)
{
    xoffset *= MouseSensitivity * GetFrameTime();
    yoffset *= MouseSensitivity * GetFrameTime();

    Yaw += xoffset;
    Pitch -= yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped.
    if (constrainPitch)
        Pitch = Clamp(Pitch, -89.0f, 89.0f);
    UpdateCameraVectors(camera);
}

void UpdateZoom(Camera3D* camera, int yoffset)
{
    if (camera->fovy >= 1.0f && camera->fovy <= 45.0f)
        camera->fovy -= yoffset;
    if (Zoom <= 1.0f)
        camera->fovy = 1.0f;
    if (camera->fovy >= 45.0f)
        camera->fovy = 45.0f;
}

// Move camera around in 3D.
Vector3 UpdateMovement(float speed, float dt)
{
    Vector3 direction = Vector3Zero();
    if (IsKeyDown(KEY_W))
        direction = Vector3Add(direction, Front);
    if (IsKeyDown(KEY_A))
        direction = Vector3Add(direction, Vector3Negate(Right));
    if (IsKeyDown(KEY_S))
        direction = Vector3Add(direction, Vector3Negate(Front));
    if (IsKeyDown(KEY_D))
        direction = Vector3Add(direction, Right);
    return Vector3Scale(direction, speed * dt);
}

void UpdateCameraCustom(Camera* camera, Vector2 mouseDelta, float dt)
{
    // Speed up/slow down.
    if (IsKeyPressed(KEY_LEFT_SHIFT))
        CurrentSpeed = MovementSpeed * 2.0f;
    if (IsKeyReleased(KEY_LEFT_SHIFT))
        CurrentSpeed = MovementSpeed;

    // Flying first person camera movement.
    Vector3 movementDelta = UpdateMovement(CurrentSpeed, dt);
    camera->position = Vector3Add(camera->position, movementDelta);

    // Look around using mouse movement.
    if (mouseDelta.x != 0 || mouseDelta.y != 0)
        UpdateMouseMovement(camera, mouseDelta.x, mouseDelta.y);

    camera->target = Vector3Add(camera->position, Front);

    // Zooming in and out with the scroll wheel.
    int scrollY = GetMouseWheelMove();
    if (scrollY != 0)
        UpdateZoom(camera, scrollY);
}
