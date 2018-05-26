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
void GameState::Initiate(int spriteTiles, int spritesterPlayer, int spritesterEnemy){
    sprites = spriteTiles;
    spritePlayer = spritesterPlayer;
    spriteEnemy = spritesterEnemy;
    LoadLevel();
}


void GameState::LoadLevel () {
    if (mappy != nullptr) {
        delete mappy;
    }
    mappy = new FlareMap();
    enemies.clear ();
    std::stringstream stream;
    stream << "NYUCodebase.app/Contents/Resources/Resources/FinalLevel" << level << ".txt";
    mappy -> Load (stream.str());
    for (size_t index = 0; index < mappy -> entities.size (); index++ ){
        //create the player
        if (mappy -> entities [index].type == "playerRed") {
            
            player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850, 518, 39, 48, TILE_SIZE);
            player.gravity.y = -2.5f;
        }
        
        if (mappy -> entities [index].type == "playerBlue") {
            player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 762, 203, 45, 54, TILE_SIZE);
            player.gravity.y = -2.5f;

        }
        
        // creating the enemies
        if (mappy -> entities [index].type == "spider") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 929, 949, 32, 44, TILE_SIZE));
        }
        
        if (mappy -> entities [index].type == "keyRed") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 962, 252, 29, 30, TILE_SIZE /2 );
        }
        
        if (mappy -> entities [index].type == "keyGreen") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961, 495, 29, 30, TILE_SIZE /2);
        }
    }
}

void GameState::UpdateLevel() {
    level++;
    keyObtained = false;
    LoadLevel();
}

int GameState::GetLevel(){
    return level;
}

// drawing game
void GameState::Draw (ShaderProgram* program) {
    mappy -> Draw (program, sprites);
    player.Draw (program);
    for (int i = 0; i < enemies.size (); i++){
        enemies[i].Draw (program);
    }
    key.Draw (program);
}

// checking for any collisions in game
void GameState::CollisionEntities () {
    

    // if collision between enemy and player
    // reset player to start of the game
    for (int index = 0; index < enemies.size (); index++) {
        if (player.Collision(&enemies[index]) ) {
            player.position.x = 1 * TILE_SIZE;
            player.position.y = 19.5 * -1 * TILE_SIZE;
        }
    }
    
    if (player.Collision (&key)) {
        keyObtained = true;
        key.position.x = -3.55;
    }
    
    int TileX;
    int TileY;
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    if (keyObtained && (mappy -> mapData [TileY] [TileX] - 1 == 152 || mappy -> mapData [TileY] [TileX] - 1 == 153)) {
        UpdateLevel ();
    }
    
}

void GameState::CollisionX () {
    int TileX;
    int TileY;
    int TileLeftX;
    int TileRightX;
    int TileTopY;
    int TileBottomY;
    
    bool rightTile = false;
    bool leftTile = false;
    
    // calculate the above Tile values of center, left, right, top, bottom
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    player.worldToTileCoordinates(player.position.x - player.sizeEnt.x/2, player.position.y - player.sizeEnt.y/2 , &TileLeftX, &TileBottomY);
    player.worldToTileCoordinates(player.position.x + player.sizeEnt.x/2, player.position.y + player.sizeEnt.y/2, &TileRightX, &TileTopY);
    
    for (int x: solidTiles) {
        if (x == mappy -> mapData [TileY][TileRightX] - 1) {
            rightTile = true;
        }
        if (x == mappy -> mapData [TileY][TileLeftX] - 1) {
            leftTile = true;
        }
    }
    
    if (rightTile) {
        float worldRightX = TILE_SIZE * TileRightX;
        if (worldRightX < player.position.x + player.sizeEnt.x/2) {
            player.acceleration.x = 0.0f;
            player.velocity.x = 0.0f;
            player.position.x -= (player.position.x + player.sizeEnt.x/2 - worldRightX) ;
        }
        player.collidedRight = true;
    }
    
    // if left tile is solid
    if (leftTile) {
        float worldLeftX = TILE_SIZE * TileLeftX;
        if (worldLeftX + TILE_SIZE > player.position.x - player.sizeEnt.x/2) {
            player.acceleration.x = 0.0f;
            player.velocity.x = 0.0f;
            player.position.x += (worldLeftX - player.position.x - player.sizeEnt.x/2) + 2 * TILE_SIZE;
        }
        player.collidedLeft = true;
    }
}

void GameState::CollisionY () {
    
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
    
    bool bottomTile = false;
    bool topTile = false;

    for (int x: solidTiles) {
        if (x == mappy -> mapData[TileBottomY] [TileX] - 1) {
            bottomTile = true;
        }
        if (x == mappy -> mapData [TileTopY][ TileX] - 1) {
            topTile = true;
        }
    }
    
 
    // if tile below is solid, reset bottom to be on top
    if (bottomTile) {
        float worldBotY = -1 * TILE_SIZE * TileBottomY;
        if (worldBotY > player.position.y - player.sizeEnt.y/2) {
            player.acceleration.y = 0.0f;
            player.velocity.y = 0.0f;
            player.position.y += (worldBotY - player.position.y - player.sizeEnt.y/2) + TILE_SIZE;
        }
        player.collidedBottom = true;
    }
    
    
    // if tile above is solid
    if (topTile) {
        float worldTopY = -1 * TILE_SIZE * TileTopY;
        if (worldTopY - TILE_SIZE < player.position.y + player.sizeEnt.y/2) {
            player.acceleration.y = 0.0f;
            player.velocity.y = 0.0f;
            player.position.y -= (player.position.y + player.sizeEnt.y/2 - worldTopY) + TILE_SIZE;
        }
         player.collidedTop = true;
    }
  
}



