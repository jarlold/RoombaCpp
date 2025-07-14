#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "room.cpp"
#include <vector>
#include <cstdio>

void draw(Camera2D camera, std::vector<Roomba>& roombas, Room& room) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(10, 10);
    BeginMode2D(camera);
        rlPushMatrix();
            rlTranslatef(50*50, 50*50, 0);
            rlRotatef(90, 1, 0, 0);
            DrawGrid(100, 100);
        rlPopMatrix();
        
        drawRoom(room);
        drawRoombas(roombas);
        
    EndMode2D();
    EndDrawing();
}

int main() {
    // Setup the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Roomba CPP Version");

    // Set up the camera
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.target = {-screenWidth/2, -screenHeight/2};
    SetTargetFPS(60);
    
    // Make a room
    Room room = buildRoom(800.0, 800.0);
    populateRoom(room, 1);
    
    // Let's make a roomba!
    std::vector<Roomba> roombas = generateNRoombas(room, 4, room.boundingBox.width/2);
    
    while (!WindowShouldClose()) {
        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }
        
        // Zoom in and out with the mouse wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            // Translate the mouse's window position into the canvas coordinate system
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            
            // We'll pan towards the mouse as we zoom in and out
            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;
            
            // Increment zoom in log scale
            float scale = 0.2f*wheel;
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
        }
        
        // Enter the main draw function
        draw(camera, roombas, room);
        
        // Update the roombas
        updateRoombas(camera, roombas, room, 1.0/60); 
    }
    
    CloseWindow();
    return 0;
}
