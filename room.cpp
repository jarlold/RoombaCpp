#pragma once

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>

#define ROOMBA_SIZE 20

struct Roomba {
    Vector2 position;
    Vector2 velocity;
    double bearing;
    Color color;
};

struct Wall {
    Rectangle boundingBox;
    Color color;
};

struct Room {
    Rectangle boundingBox;
    std::vector<Wall> walls;
};


Color generateNiceColor() {
    int f = 150;
    int m = 255 - f;
    unsigned char r = f + GetRandomValue(0, m);
    unsigned char g = f + GetRandomValue(0, m);
    unsigned char b = f + GetRandomValue(0, m);
    Color c = {r, g, b, 255};
    return c;
}

Roomba buildRoomba(Vector2 position) {
    Vector2 velocity = {0.0, 0.0};
    double bearing = GetRandomValue(0, 360) / (2*PI);
    Color color = generateNiceColor();    
    Roomba r = {position, velocity, bearing, color};
    return r;
}


bool roombaInAWall(Room& room, Roomba& roomba) {
    // Naive O(n^2) approach for bounds detection
    for (Wall w : room.walls) {
        if (
            roomba.position.x + ROOMBA_SIZE > w.boundingBox.x &&
            roomba.position.x - ROOMBA_SIZE < w.boundingBox.x + w.boundingBox.width &&
            roomba.position.y + ROOMBA_SIZE > w.boundingBox.y && 
            roomba.position.y - ROOMBA_SIZE < w.boundingBox.y + w.boundingBox.height 
        ) return true;
    }
    return false;
}

std::vector<Roomba> generateNRoombas(Room& room, int n, float radius) {
    std::vector<Roomba> roombas(n);
    for (int i=0; i<n; i++) {
        // LETS GO GAMBLING!!!
        do {
            // Get a random position in a circle around 0,0 of radius whatever
            Vector2 f = {
                (float) (GetRandomValue(-255, 255)/510.0f),
                (float) (GetRandomValue(-255, 255)/510.0f)
            };
            f = Vector2Scale(Vector2Normalize(f), GetRandomValue(-radius, radius));
            roombas[i] = buildRoomba( (Vector2) {f.x, f.y} );
        } while ( roombaInAWall(room, roombas[i]) );
    }
    return roombas;
}

void drawRoomba(Roomba& r) {
    DrawCircleV(r.position, ROOMBA_SIZE, r.color);
    DrawCircle(r.position.x + cos(r.bearing)*10, r.position.y + sin(r.bearing)*10, ROOMBA_SIZE/5, RAYWHITE);
}

void drawRoombas(std::vector<Roomba>& roombas) {
    int b = roombas.size();
    for (int i=0; i < b; i++) {
        drawRoomba(roombas[i]);
    }
}

Room buildRoom(float width, float height) {
    Vector2 topLeft = {-width/2.0f, -height/2.0f};
    Vector2 size = {width, height};
    std::vector<Wall> walls;
    Room r = (Room) { (Rectangle) {topLeft.x, topLeft.y, size.x, size.y}, walls };
    return r;
}


void populateRoom(Room& room, int n) {
    for (int i=0; i<n; i++) {
        Rectangle r = {room.boundingBox.x, room.boundingBox.y, room.boundingBox.width/4, room.boundingBox.height/4};
        Wall new_wall = {r, SKYBLUE};
        room.walls.push_back(new_wall);
    }
}

void drawRoom(Room& room) {
    DrawRectangleRec(room.boundingBox, (Color) {0xe9, 0xe9, 0xe9, 0xff});
    for (Wall w : room.walls) {
        DrawRectangleRec(w.boundingBox, WHITE);
        DrawRectangleRoundedLinesEx(w.boundingBox, 0.05, 10, 4.0, BLACK);
    }
    DrawRectangleRoundedLinesEx(room.boundingBox, 0.01, 10, 4.0, BLACK);
}

bool roombaOutOfRoom(Room& room, Roomba& roomba) {
    // Check to make sure THE WHOLE ROOMBA is inside the walls
    return ( 
        roomba.position.x - ROOMBA_SIZE < room.boundingBox.x ||
        roomba.position.x + ROOMBA_SIZE > room.boundingBox.x + room.boundingBox.width ||
        roomba.position.y - ROOMBA_SIZE < room.boundingBox.y ||
        roomba.position.y + ROOMBA_SIZE > room.boundingBox.y + room.boundingBox.height
    );
}

void updateRoomba(Camera2D& camera, Roomba& roomba, Room& room, float dt) {
    // Roomba will face friction
    roomba.velocity = Vector2Scale(roomba.velocity, 0.99);
    
    // Roomba will accelerate towards the mouse as best he can
    Vector2 accel = GetScreenToWorld2D(GetMousePosition(), camera);
    accel = Vector2Subtract(roomba.position, accel);
    accel = Vector2Normalize(accel);
    
    // TODO: The bearing should decide the direction, not the otherway around
    roomba.bearing = atan2(roomba.velocity.y, roomba.velocity.x);
    
    accel = Vector2Scale(accel, -200);
    
    // We do a little frame numerical integration, it's called we do a little frame numerical integration
    roomba.velocity = Vector2Add(Vector2Scale(accel, dt), roomba.velocity);

    // Update the roombas position, except if that takes him out of bounds then don't
    roomba.position.x += roomba.velocity.x*dt;
    if (roombaOutOfRoom(room, roomba) || roombaInAWall(room, roomba)) {
        roomba.position.x -= roomba.velocity.x*dt;
        roomba.velocity.x = 0;
    }
    
    roomba.position.y += roomba.velocity.y*dt;
    if (roombaOutOfRoom(room, roomba) || roombaInAWall(room, roomba)) {
        roomba.position.y -= roomba.velocity.y*dt;
        roomba.velocity.y = 0;
    }
}

void updateRoombas(Camera2D& camera, std::vector<Roomba>& roombas, Room& room, float dt) {
    int b = roombas.size();
    for (int i=0; i< b; i++) {
        updateRoomba(camera, roombas[i], room, dt);
    }
}

