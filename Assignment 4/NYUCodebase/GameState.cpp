//
//  GameState.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 3/6/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "GameState.h"
#include "Entity.h"
#include "SheetSprite.h"
#include "FlareMap.h"
#include <vector>
#include <string>
#define TILE_SIZE 0.2f

GameState::GameState () {}

// initializing player and enemies
void GameState::Initiate(int spritester, const std::string fileName){
    sprites = spritester;
    mappy.Load (fileName);

    for (size_t index = 0; index < mappy.entities.size (); index++ ){
        //create the player
        if (mappy.entities [index].type == "Player") {
            player = Entity (sprites, 99, mappy.entities[index].x * TILE_SIZE, (mappy.entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, TILE_SIZE);
        }
        
        // provide starting velocity y and acceleration y for birds
        else if (mappy.entities [index].type == "Bird") {
            enemies.push_back (Entity (sprites, 62 ,mappy.entities [index].x * TILE_SIZE, (mappy.entities[index].y) * -1 * TILE_SIZE + 1.0f, 0.0f, TILE_SIZE));
            enemies [ enemies.size () - 1].velocity.y = 1.0f;
            enemies [ enemies.size () - 1].acceleration.y = 0.5f;
        }
        
        // else would be the key
        else {
            key = Entity (sprites, 86, mappy.entities[index].x * TILE_SIZE, (mappy.entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, TILE_SIZE);
            
        }
        
    }
    
}

// drawing game
void GameState::Draw (ShaderProgram* program) {
    mappy.Draw (program, sprites);
    player.Draw (program);
    key.Draw (program);
    for (int i = 0; i < enemies.size (); i++){
        enemies[i].Draw (program);
    }
}

// checking for any collisions in game
void GameState::Collision () {
    
    // if collision between enemy birds and player
    // reset player to start of the game
    for (int index = 0; index < enemies.size (); index++) {
        if (player.Collision(&enemies[index]) ) {
            player.position.x = 1 * TILE_SIZE;
            player.position.y = 19.5 * -1 * TILE_SIZE;
        }
    }
    
    int TileX;
    int TileY;
    int TileLeftX;
    int TileRightX;
    int TileTopY;
    int TileBottomY;
    // calculate the above Tile values of center, left, right, top, bottom
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    player.worldToTileCoordinates(player.position.x - player.sizeEnt.x/2, player.position.y - player.sizeEnt.y/2 , &TileLeftX, &TileBottomY);
    player.worldToTileCoordinates(player.position.x + player.sizeEnt.x/2, player.position.y + player.sizeEnt.y/2, &TileRightX, &TileTopY);
    
    // if tile below is 0, free fall
    if (mappy.mapData[TileBottomY] [TileX] == 0) {
        player.gravity.y = -0.55f;
        player.velocity.y = -1.0f;
    }
    
    // if tile below is spikes, restart to the beginning
    if (mappy.mapData [TileBottomY][ TileX] == 101) {
        player.position.x = 0.2f;
        player.position.y = 19.5 * -1 * TILE_SIZE;
    }
    
    // if tile below is solid, reset bottom to be on top
    else if (mappy.mapData [TileBottomY][ TileX] != 0) {
        float worldBotY = -1 * TILE_SIZE * TileBottomY;
        if (worldBotY > player.position.y - player.sizeEnt.y/2) {
            player.acceleration.y = 0.0f;
            player.velocity.y = 0.0f;
            player.position.y += (worldBotY - player.position.y - player.sizeEnt.y/2) + TILE_SIZE;
        }
    }
    
    
    // if tile above is solid
    if (mappy.mapData [TileTopY][ TileX] != 0) {
        float worldTopY = -1 * TILE_SIZE * TileTopY;
        if (worldTopY - 0.2f < player.position.y + player.sizeEnt.y/2) {
            player.acceleration.y = 0.0f;
            player.velocity.y = 0.0f;
            player.position.y -= (player.position.y + player.sizeEnt.y/2 - worldTopY) + TILE_SIZE;
        }
    }
    
    // if right tile is solid
    if (mappy.mapData [TileY][TileRightX] != 0) {
        float worldRightX = TILE_SIZE * TileRightX;
        if (worldRightX < player.position.x + player.sizeEnt.x/2) {
            player.acceleration.x = 0.0f;
            player.velocity.x = 0.0f;
            player.position.x -= (player.position.x + player.sizeEnt.x/2 - worldRightX) ;
        }
    }
    
    // if left tile is solid
    if (mappy.mapData [TileY][TileLeftX] != 0) {
        float worldLeftX = TILE_SIZE * TileLeftX;
        if (worldLeftX + TILE_SIZE > player.position.x - player.sizeEnt.x/2) {
            player.acceleration.x = 0.0f;
            player.velocity.x = 0.0f;
            player.position.x += (worldLeftX - player.position.x - player.sizeEnt.x/2) + 0.4f;
        }
    }
}




