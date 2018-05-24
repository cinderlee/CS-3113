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
#define TILE_SIZE 0.3f

GameState::GameState () {}

// initializing player and enemies
void GameState::Initiate(int spriteTiles, int spritesterPlayer, int spritesterEnemy, const std::string fileName){
    sprites = spriteTiles;
    spritePlayer = spritesterPlayer;
    spriteEnemy =spritesterEnemy;
    mappy.Load (fileName);

    for (size_t index = 0; index < mappy.entities.size (); index++ ){
        //create the player
        if (mappy.entities [index].type == "playerRed") {
            
            player = Entity (spritePlayer, (mappy.entities[index].x + 0.5f) * TILE_SIZE, (mappy.entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850, 518, 39, 48, TILE_SIZE);
        }
        
        // creating the enemies
        if (mappy.entities [index].type == "spider") {
            enemies.push_back (Entity (spritePlayer, (mappy.entities [index].x + 0.5) * TILE_SIZE, (mappy.entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 929, 949, 32, 44, TILE_SIZE));
            // enemies [ enemies.size () - 1].velocity.y = 1.0f;
            // enemies [ enemies.size () - 1].acceleration.y = 0.5f;
        }
    }

}

// drawing game
void GameState::Draw (ShaderProgram* program) {
    mappy.Draw (program, sprites);
    player.Draw (program);
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
        if (worldTopY - TILE_SIZE < player.position.y + player.sizeEnt.y/2) {
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
            player.position.x += (worldLeftX - player.position.x - player.sizeEnt.x/2) + 2 * TILE_SIZE;
        }
    }
    
}




