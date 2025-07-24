#pragma once

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "neural_network.cpp"

#include <vector>
#include <cmath>
#include <climits> 

#define ROOMBA_SIZE 20
#define NUM_MEMORY 4
#define DUST_SPACING 10

struct Roomba {
    Vector2 position;
    Vector2 velocity;
    float bearing;
    float speed;
    Color color;
    long dirtEaten;
    NeuralNetworks::NeuralNetwork neuralNetwork;
    std::array<float, NUM_MEMORY> memory; // for storing traumatic events
    std::vector<std::vector<bool>> usedDustPositions; // there has to be a better way to do this, but this is how I did it in the
                                                      // original simulator.
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

Roomba buildRoomba(Vector2 position, NeuralNetworks::NeuralNetwork& nn, std::vector<std::vector<bool>>& dustPositions) {
    Vector2 velocity = {0.0, 0.0};
    float bearing = GetRandomValue(0, 360) / (2*PI);
    std::array<float, NUM_MEMORY> memories = {0};
    Color color = generateNiceColor();
    // My very advanced super safe totally not gonna collide id generating system   
    Roomba r = {position, velocity, bearing, 25.0f, color, 0, nn, memories, dustPositions};
    return r;
}

void drawRoomba(Roomba& r) {
    DrawCircleV(r.position, ROOMBA_SIZE, r.color);
    DrawCircle(r.position.x + cos(r.bearing)*10, r.position.y + sin(r.bearing)*10, ROOMBA_SIZE/5, RAYWHITE);
    
    // This is probably slow but i don't want to fiddle with buffers 
    std::string s = std::to_string(r.dirtEaten);
    DrawText(s.c_str(), r.position.x, r.position.y, 6, BLACK);
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


// Fill the room with walls until we had fillCapacity% full, or n walls. Whichever comes first.
void populateRoom(Room& room, int n, float fillCapacity) {
    std::vector<Wall> walls;
    int done = 0;
    int strides = 100;
    int howManyX = floor(room.boundingBox.width/strides);
    int howManyY = floor(room.boundingBox.height/strides);
    int squiggleFactor = ROOMBA_SIZE * 3;
    
    for (int i=0; i < howManyX; i++) {
        for (int j=0; j < howManyY; j++) {
            if ( ((float)(rand() % (howManyX*howManyY) )) / (howManyX*howManyY) < fillCapacity ) {
                Rectangle r = (Rectangle) { (float) strides*i, (float) strides*j, (float) strides, (float) strides};
                // Why on earth did I put this thing centered at the origin, its a hasstle!
                r.x -= room.boundingBox.width/2;
                r.y -= room.boundingBox.height/2;
                
                // Add a little bit of squiggle to it you know
                float squiggleX = (rand() % squiggleFactor) - squiggleFactor/2;
                r.x += squiggleX;
                
                float squiggleY = (rand() % squiggleFactor) - squiggleFactor/2;
                r.y += squiggleY;
                
                // TODO: maybe put a box-box check here (instead of a point-box check) for a starting zone?
                if ( 0 > i*strides && 0 < i*strides+strides && 0 > j*strides && 0 < j*strides+strides) {
                    continue;
                }
                
                walls.push_back((Wall) {r, SKYBLUE} );
                done++;
                if (done >= n) break;
            }
        }
        
        if (done >= n) break;
    }
    
    room.walls = walls;
}

void drawRoom(Room& room) {
    DrawRectangleRec(room.boundingBox, (Color) {0xe9, 0xe9, 0xe9, 0xff});
    for (Wall w : room.walls) {
        DrawRectangleRec(w.boundingBox, w.color);
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

bool roombaInAWall(Room& room, Roomba& roomba) {
    // Naive O(n^2) approach for bounds detection
    for (Wall& w : room.walls) {
        if (
            roomba.position.x + ROOMBA_SIZE > w.boundingBox.x &&
            roomba.position.x - ROOMBA_SIZE < w.boundingBox.x + w.boundingBox.width &&
            roomba.position.y + ROOMBA_SIZE > w.boundingBox.y && 
            roomba.position.y - ROOMBA_SIZE < w.boundingBox.y + w.boundingBox.height 
        ) {
            w.color = RED;
            return true;
        } else {
            //w.color = GREEN;
        }
    }
    return false;
}

float getRoombaBearingAdjustment(Roomba& roomba, Room& room) {
    /* Let's give the vaccum cleaners memories! Nothing could go wrong!
       Give them memories then make them do chores for epochs and epochs!
       Nothing bad can happen due to this! Our actions have no consequences!
    */
    std::vector<float> inputData(4 + NUM_MEMORY);
    for (int i = 0; i < NUM_MEMORY; i++) {
        inputData[i] = roomba.memory[i];
    }
    
    //TODO: Temporary placeholders, we'll have to make the roomba keep some metrics later
    // Real roombas don't get to know their X and Y so easily so....
    inputData[NUM_MEMORY+1] = roomba.position.x / (float) room.boundingBox.width;
    inputData[NUM_MEMORY+2] = roomba.position.y / (float) room.boundingBox.height;
    inputData[NUM_MEMORY+3] = roomba.velocity.x / (float) room.boundingBox.width;
    inputData[NUM_MEMORY+4] = roomba.velocity.y / (float) room.boundingBox.height;
    
    // Do the forwards pass and get the data out
    std::vector<float> output = NeuralNetworks::doPrediction(roomba.neuralNetwork, inputData);
    float newBearing = output[NUM_MEMORY];
    for (int i = 0; i < NUM_MEMORY; i++) {
        roomba.memory[i] = output[i];
    }
    
    // I only implemented sigmoid as my activation function so we'll just down
    // shift it and call it tanh
    newBearing -= 0.5;
    newBearing *= 2;
    
    // Clamp it between -n and +n
    if (newBearing > PI)
        newBearing = PI;
    else if (newBearing < -PI)
        newBearing = -PI;
    
    return newBearing;
}

void updateRoomba(Roomba& roomba, Room& room, float dt) {
    // Roomba will adjust his bearing based on the output of his neural network
    roomba.bearing += getRoombaBearingAdjustment(roomba, room) * dt;
    
    // Make sure his bearing isn't in some stupid range.
    while (roomba.bearing > 2 * PI) roomba.bearing -= 2 * PI;
    while (roomba.bearing < 0 ) roomba.bearing += 2 * PI;

    // Roomba will face friction
    roomba.velocity = Vector2Scale(roomba.velocity, 0.99);
    
    // Roomba will accelerate in the direction of his bearing
    Vector2 accel = (Vector2) { (float) cos(roomba.bearing), (float) sin(roomba.bearing) };
    accel = Vector2Scale(accel, roomba.speed);
    
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
    
    
    // Now we'll say there's bits of dirt at every spot in an N by N grid
    // and when a roomba passes over such a spot he gets to eat it. Yum yum!
    
    // I forgot about the signs when I was setting this up, so we have to shift things
    // to be positive. I think this should work.
    float trueX = floor((roomba.position.x+room.boundingBox.width/2)/DUST_SPACING);
    float trueY = floor((roomba.position.y + room.boundingBox.height/2)/DUST_SPACING);
    
    // But if it doesn't this will let me know when I screwed up. That and the segfault of
    // course.
    if (trueX < 0 || trueY < 0) {
        roomba.color = RED;
    }

    // Reward the roomba for finding unique dirt.
    if(!roomba.usedDustPositions[trueX][trueY]) {
        roomba.dirtEaten++; // Good boy roomba :D
        roomba.usedDustPositions[trueX][trueY] = true;
    }
}

void updateRoombas(std::vector<Roomba>& roombas, Room& room, float dt) {
    int b = roombas.size();
    for (int i=0; i< b; i++) {
        updateRoomba(roombas[i], room, dt);
    }
}

// Mostly just for fun, not used for actualy genetic algorithm
std::vector<Roomba> generateNRoombas(Room& room, int n, float radius) {
    std::vector<Roomba> roombas(n);
    std::vector<int> layerSizes = { 4 + NUM_MEMORY, 5, 6, 5, 4 + NUM_MEMORY };
    for (int i=0; i<n; i++) {
        // Generate a neural network where all the weights are 1.0
        NeuralNetworks::ActivationFunc f = &NeuralNetworks::sigmoidVec;
        NeuralNetworks::NeuralNetwork nn = NeuralNetworks::buildNeuralNetwork(layerSizes, f);
        tweakNeuralNetwork(nn, 1.0, 20);
        int m = (int)(room.boundingBox.width/DUST_SPACING);
        int n = (int)(room.boundingBox.height/DUST_SPACING);
        std::vector<std::vector<bool>> dustPositions(m, std::vector<bool>(n, false));
    
        // LETS GO GAMBLING!!!
        // TODO: Cast a ray instead and see if it intersects with a wall, if so
        // stop, move ROOMBA_SIZE backwards, and put the roomba there
        // otherwise we're gonna spend A LOT of time allocating dustPositions lol
        do {
            // Get a random position in a circle around 0,0 of radius whatever
            Vector2 f = {
                (float) (GetRandomValue(-255, 255)/510.0f),
                (float) (GetRandomValue(-255, 255)/510.0f)
            };
            f = Vector2Scale(Vector2Normalize(f), GetRandomValue(-radius, radius));
            roombas[i] = buildRoomba( (Vector2) {f.x, f.y}, nn, dustPositions);
        } while ( roombaInAWall(room, roombas[i]) && false);
    }
    return roombas;
}


