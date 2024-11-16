#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function prototypes
int titleScreen(void);
int start_Game(void);
int credits(void);

int main(void) {
    titleScreen();
    return 0;
}

//------------------------------------------------------------------------------------
// Ttile Screen main entry point
//------------------------------------------------------------------------------------

int titleScreen(void) {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Welcome to DOOM!");

    // Initialize the 3D camera
    Camera camera = { 0 };
    camera.position = (Vector3){ 16.0f, 14.0f, 16.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Load cubic map resources
    Image image = LoadImage("C:/Users/SyedA/Desktop/doom/resources/Main_Game/cubicmac_Mesh.png");
    Texture2D cubicmap = LoadTextureFromImage(image);
    Mesh mesh = GenMeshCubicmap(image, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model model = LoadModelFromMesh(mesh);
    Texture2D texture = LoadTexture("C:/Users/SyedA/Desktop/doom/resources/Main_Game/cubicmap_atlas.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };
    UnloadImage(image);

    // Button positions and sizes
    Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 100, 200, 50 };
    Rectangle creditsButton = { screenWidth / 2 - 100, screenHeight / 2, 200, 50 };
    Rectangle exitButton = { screenWidth / 2 - 100, screenHeight / 2 + 100, 200, 50 };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePoint, startButton)) {
                start_Game();
            } else if (CheckCollisionPointRec(mousePoint, creditsButton)) {
                credits();
            } else if (CheckCollisionPointRec(mousePoint, exitButton)) {
                break;
            }
        }

        // Update camera to rotate around the map
        UpdateCamera(&camera, CAMERA_ORBITAL);

        // Draw
        BeginDrawing();
            ClearBackground(DARKBROWN);

            // Draw the rotating 3D scene
            BeginMode3D(camera);
                DrawModel(model, mapPosition, 1.0f, WHITE);
            EndMode3D();

            // Draw buttons
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawRectangleRec(creditsButton, LIGHTGRAY);
            DrawRectangleRec(exitButton, LIGHTGRAY);

            // Button text
            DrawText("Start Game", screenWidth / 2 - MeasureText("Start Game", 20) / 2, screenHeight / 2 - 90, 20, BLACK);
            DrawText("Credits", screenWidth / 2 - MeasureText("Credits", 20) / 2, screenHeight / 2 + 10, 20, BLACK);
            DrawText("Exit Game", screenWidth / 2 - MeasureText("Exit Game", 20) / 2, screenHeight / 2 + 110, 20, BLACK);

            DrawFPS(10, 10);
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(cubicmap);
    UnloadTexture(texture);
    UnloadModel(model);
    CloseWindow();

    return 0;
}

//------------------------------------------------------------------------------------
// Game main entry point
//------------------------------------------------------------------------------------


int start_Game(void) {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Welcome to Doom!");

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.6f, 0.6f, 0.6f };
    camera.target = (Vector3){ 0.185f, 0.4f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Image imMap = LoadImage("C:/Users/SyedA/Desktop/doom/resources/Main_Game/cubicmac_Mesh.png");
    Texture2D cubicmap = LoadTextureFromImage(imMap);
    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model model = LoadModelFromMesh(mesh);

    Texture2D texture = LoadTexture("C:/Users/SyedA/Desktop/doom/resources/Main_Game/cubicmap_atlas.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    Color *mapPixels = LoadImageColors(imMap);
    UnloadImage(imMap);

    Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };
    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector3 oldCamPos = camera.position;
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        Vector2 playerPos = { camera.position.x, camera.position.z };
        float playerRadius = 0.1f;
        int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
        int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

        if (playerCellX < 0) playerCellX = 0;
        else if (playerCellX >= cubicmap.width) playerCellX = cubicmap.width - 1;

        if (playerCellY < 0) playerCellY = 0;
        else if (playerCellY >= cubicmap.height) playerCellY = cubicmap.height - 1;

        for (int y = 0; y < cubicmap.height; y++) {
            for (int x = 0; x < cubicmap.width; x++) {
                if ((mapPixels[y*cubicmap.width + x].r == 255) &&
                    (CheckCollisionCircleRec(playerPos, playerRadius, (Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f }))) {
                    camera.position = oldCamPos;
                }
            }
        }

        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode3D(camera);
                DrawModel(model, mapPosition, 1.0f, WHITE);
            EndMode3D();
            DrawTextureEx(cubicmap, (Vector2){ GetScreenWidth() - cubicmap.width*4.0f - 20, 20.0f }, 0.0f, 4.0f, WHITE);
            DrawRectangleLines(GetScreenWidth() - cubicmap.width*4 - 20, 20, cubicmap.width*4, cubicmap.height*4, GREEN);
            DrawRectangle(GetScreenWidth() - cubicmap.width*4 - 20 + playerCellX*4, 20 + playerCellY*4, 4, 4, RED);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadImageColors(mapPixels);
    UnloadTexture(cubicmap);
    UnloadTexture(texture);
    UnloadModel(model);
    CloseWindow();

    return 0;
}

// Credits Screen (Placeholder)
int credits(void) {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Credits");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Credits", screenWidth / 2 - MeasureText("Credits", 40) / 2, screenHeight / 2 - 20, 40, BLACK);
            DrawText("Press ESC to return", screenWidth / 2 - MeasureText("Press ESC to return", 20) / 2, screenHeight / 2 + 30, 20, GRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE)) break;  // Exit to title screen
    }

    CloseWindow();
    return 0;
}
