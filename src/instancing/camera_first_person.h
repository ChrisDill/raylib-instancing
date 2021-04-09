#ifndef CAMERA_FIRST_PERSON_H
#define CAMERA_FIRST_PERSON_H

#include "raylib.h"
#include "raymath.h"

// Camera constants
const float MovementSpeed = 20.5f;
const float MouseSensitivity = 15.0f;

typedef struct CameraFP  {
    Camera3D view;
    float currentSpeed;
    Vector3 front;
    Vector3 right;
    Vector3 up;
    float yaw;
    float pitch;
    float zoom;
    bool constrainPitch;
} CameraFP;

CameraFP LoadCameraFP()
{
    CameraFP camera = {
        .currentSpeed = MovementSpeed,
        .front = { 0.0f, 0.0f, -1.0f },
        .right = { 1.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .yaw = -90.0f,
        .pitch = 0.0f,
        .zoom = 45.0f,
        .constrainPitch = true
    };

    // Define the camera to look into our 3d world
    camera.view = (Camera3D) {
        .position = (Vector3){ 0.0f, 0.0f, 0.0f },
        .target = (Vector3) { 0.0f, 0.0f, 0.0f },
        .up = (Vector3) { 0.0f, 1.0f, 0.0f },
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    SetCameraMode(camera.view, CAMERA_CUSTOM);

    return camera;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void UpdateCameraVectors(CameraFP* camera)
{
    // Calculate the new Front vector
    Vector3 front = {};
    front.x = cos(DEG2RAD * camera->yaw) * cos(DEG2RAD * camera->pitch);
    front.y = sin(DEG2RAD * camera->pitch);
    front.z = sin(DEG2RAD * camera->yaw) * cos(DEG2RAD * camera->pitch);
    camera->front = Vector3Normalize(front);

    // Also re-calculate the right and up vector
    // Normalize the vectors, because their length gets closer to 0
    // the more you look up or down which results in slower movement.
    camera->right = Vector3Normalize(Vector3CrossProduct(camera->front, (Vector3){ 0, 1, 0 }));
    camera->up = Vector3Normalize(Vector3CrossProduct(camera->right, camera->front));
}

// Adjust yaw and pitch to look around and contrain pitch(up and down).
void UpdateMouseMovement(CameraFP* camera, int xoffset, int yoffset)
{
    xoffset *= MouseSensitivity * GetFrameTime();
    yoffset *= MouseSensitivity * GetFrameTime();

    camera->yaw += xoffset;
    camera->pitch -= yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped.
    if (camera->constrainPitch)
        camera->pitch = Clamp(camera->pitch, -89.0f, 89.0f);
    UpdateCameraVectors(camera);
}

void UpdateZoom(Camera3D* camera, float zoom, int yoffset)
{
    if (camera->fovy >= 1.0f && camera->fovy <= 45.0f)
        camera->fovy -= yoffset;
    if (zoom <= 1.0f)
        camera->fovy = 1.0f;
    if (camera->fovy >= 45.0f)
        camera->fovy = 45.0f;
}

// Move camera around in 3D.
Vector3 GetMovement(CameraFP *camera, float dt)
{
    Vector3 direction = Vector3Zero();
    if (IsKeyDown(KEY_W))
        direction = Vector3Add(direction, camera->front);
    if (IsKeyDown(KEY_A))
        direction = Vector3Add(direction, Vector3Negate(camera->right));
    if (IsKeyDown(KEY_S))
        direction = Vector3Add(direction, Vector3Negate(camera->front));
    if (IsKeyDown(KEY_D))
        direction = Vector3Add(direction, camera->right);

    if (IsKeyDown(KEY_SPACE))
        direction = Vector3Add(direction, camera->up);
    if (IsKeyDown(KEY_LEFT_SHIFT))
        direction =  Vector3Add(direction, Vector3Negate(camera->up));

    return Vector3Scale(direction,  camera->currentSpeed * dt);
}

void UpdateCameraCustom(CameraFP* camera, Vector2 mouseDelta, float dt)
{
    // Speed up/slow down
    if (IsKeyPressed(KEY_LEFT_CONTROL))
        camera->currentSpeed = MovementSpeed * 2.0f;
    if (IsKeyReleased(KEY_LEFT_CONTROL))
        camera->currentSpeed = MovementSpeed;

    // Flying first person camera movement
    Vector3 movementDelta = GetMovement(camera, dt);
    camera->view.position = Vector3Add(camera->view.position, movementDelta);

    // Look around using mouse movement
    if (mouseDelta.x != 0 || mouseDelta.y != 0)
        UpdateMouseMovement(camera, mouseDelta.x, mouseDelta.y);

    camera->view.target = Vector3Add(camera->view.position, camera->front);

    // Zoom in and out with the scroll wheel
    int scrollY = GetMouseWheelMove();
    if (scrollY != 0)
        UpdateZoom(&camera->view, camera->zoom, scrollY);
}

#endif // CAMERA_FIRST_PERSON_H
