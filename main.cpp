#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "room.cpp"
#include "neuroevolution.cpp"
#include "gui.cpp"
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

int testRoomba(NeuroEvolution::Solution& roombaSolution) {
    // We'll start by building a room.
    Room room = buildRoom(800, 800);
    populateRoom(room, 50, 0.2);
    Vector2 pos = (Vector2) {0.0, 0.0};
    
    // Properly sized dust positions are required of course. We have to make them
    // now since it requires room information.
    int m = (int)(room.boundingBox.width/DUST_SPACING);
    int n = (int)(room.boundingBox.height/DUST_SPACING);
    std::vector<std::vector<bool>> dustPositions(m, std::vector<bool>(n, false));
    
    // Finally we can birth a roomba.
    Roomba roomba = buildRoomba(pos, roombaSolution.neuralNetwork, dustPositions);
    
    // One must imagine Sisyphus happy.
    for (int i=0; i < 120*60; i++) {
        updateRoomba(roomba, room, 0.2);
    }
    
    return roomba.dirtEaten;
}

int main() {
    /*
    // Make some roomba and evolve them or whatever kids do these days
    std::vector<NeuroEvolution::Solution> sols = NeuroEvolution::generateNSolutions(5, 4+4, 10, 5, 10, 4+1);
    NeuroEvolution::testSolutions(sols, testRoomba);
    
    for (int i =0; i < sols.size(); i++) {
        printf("%d got %d dirts\r\n", i, sols[i].score);
    }
    
    return 0;
    */


    // Setup the window
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Roomba CPP Version");
    
    // Set up the camera
    Camera2D camera = { 0 };
    camera.zoom = 0.9f;
    camera.target = {-1.1 * screenWidth/2, -1.1*screenHeight/2};
    SetTargetFPS(60);
    
    // Make a room
    Room room = buildRoom(800.0, 800.0);
    populateRoom(room, 10, 0.1);
    
    // Make some roombas
    std::vector<Roomba> roombas = generateNRoombas(room, 200, 0);
    
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
        updateRoombas(roombas, room, 1.0/60); 
    }
    
    CloseWindow();
    return 0;
}
