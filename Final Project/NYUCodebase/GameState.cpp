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
            
            player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 518/1024.0f, 39/1024.0f, 48/1024.0f, TILE_SIZE);
            player.gravity.y = -1.5f;
            player.type = "playerRed";
        }
        
        if (mappy -> entities [index].type == "playerBlue" ) {
            player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 762/1024.0f, 203/1024.0f, 45/1024.0f, 54/1024.0f, TILE_SIZE);
            player.gravity.y = -3.5f;
            player.type = "playerBlue";
            
        }
        if (mappy -> entities [index].type == "playerGreen") {
            if (lives != 0 ) {
                if (level == 4) {
                    player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 849/1024.0f, 429/1024.0f, 40/1024.0f , 39/1024.0f, TILE_SIZE);
                }
                else {
                    player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 890/1024.0f, 0/1024.0f, 38/1024.0f, 50/1024.0f, TILE_SIZE);
                    player.gravity.y = -3.5f;
                }
            }
            else {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 890/1024.0f, 272/1024.0f, 38/1024.0f, 43/1024.0f, TILE_SIZE);
            }
            player.type = "playerGreen";
        }
        
        // creating the enemies
        if (mappy -> entities [index].type == "enemyWalking") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 928.0f/1024.0f, 949/1024.0f, 32/1024.0f, 44/1024.0f, TILE_SIZE));
            //928"    y="949"    width="32"    height="44
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies[enemies.size () - 1].type = "enemyGround";
        }
        
        if (mappy -> entities [index].type == "spider") {
            enemies.push_back (Entity (spriteEnemy, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 0.0f, 326/512.0f, 71/1024.0f, 45/512.0f, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies[enemies.size () - 1].type = "enemyGround";
        }
        
        if (mappy -> entities [index].type == "enemyFlying") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 706.0f/1024.0f, 839/1024.0f, 52/1024.0f, 36/1024.0f, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies[enemies.size () - 1].type = "enemyAir";
        }
        
        if (mappy -> entities [index].type == "keyRed") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 962/1024.0f, 252/1024.0f, 29/1024.0f, 30/1024.0f, TILE_SIZE /2 );
            key.type = "keyRed";
        }
        
        if (mappy -> entities [index].type == "keyGreen") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961/1024.0f, 495/1024.0f, 29/1024.0f, 30/1024.0f, TILE_SIZE /2);
            key.type = "keyGreen";
        }
    }
}

void GameState::UpdateEnemyMovement(float elapsed) {
    for (int index = 0; index < enemies.size (); index++) {
        if (enemies [index].type == "enemyGround") {
            int TileY;
            int TileLeftX;
            int TileRightX;
            int TileBottomY;
            
            bool right = false;
            bool left = false;
            
            // calculate the above Tile values of center, left, right, top, bottom
            enemies[index].worldToTileCoordinates(enemies[index].position.x + enemies[index].sizeEnt.x/2, enemies[index].position.y, &TileRightX, &TileY);
            enemies[index].worldToTileCoordinates(enemies[index].position.x - enemies[index].sizeEnt.x/2, enemies[index].position.y - enemies[index].sizeEnt.y/2 , &TileLeftX, &TileBottomY);
            
            for (int x: solidTiles) {
                if (x == mappy -> mapData [TileBottomY][TileRightX] - 1) {
                    right = true;
                }
                if (x == mappy -> mapData [TileBottomY] [TileLeftX] - 1) {
                    left = true;
                }
            }
            if ( !left || !right ) {
                enemies [index].velocity.x *= -1;
            }
            enemies[index].position.x += enemies[index].velocity.x * elapsed;
        }
        
        if (enemies [index].type == "enemyAir") {
            if (enemies [index].DistanceTo(&player) <= 1.0f) {
                float x = enemies [index].DistanceToX (&player);
                float y = enemies [index].DistanceToY (&player);
                if ( x < 0 ) {
                    std:: cout <<  "Testing Flying AI: " << x << ", " << y << std::endl;
                    enemies [index].velocity.x = 1.0f;
                    enemies [index].velocity.y = y / x;
                }
                else {
                    std:: cout <<  "Testing Flying AI2: " << x << ", " << y << std::endl;
                    enemies [index].velocity.x = -1.0f;
                    enemies [index].velocity.y = y / -x;
                }
            }
            else {
                enemies [index].velocity.x = 0.0f;
                enemies [index].velocity.y = 0.0f;
            }
            enemies [index].position.x += enemies [index].velocity.x * elapsed;
            enemies [index].position.y += enemies [index].velocity.y * elapsed;
        }
    }
}

void GameState::UpdateLevel() {
    
    level += 1;
    keyObtained = false;
    
    LoadLevel();
}

int GameState::GetLevel(){
    return level;
}

int GameState::GetLives (){
    return lives;
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
            lives--;
            if (!lives ) {
                level = 4;
            }
            else {
                level = 1;
            }
            LoadLevel();
        }
    }
    
    
    if (player.Collision (&key)) {
        keyObtained = true; 
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
            player.position.x -= (player.position.x + player.sizeEnt.x/2 - worldRightX) ;
        }
        player.collidedRight = true;
    }
    
    // if left tile is solid
    if (leftTile) {
        float worldLeftX = TILE_SIZE * TileLeftX;
        if (worldLeftX + TILE_SIZE > player.position.x - player.sizeEnt.x/2) {
            player.position.x += worldLeftX + TILE_SIZE - (player.position.x - player.sizeEnt.x/2);
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
        if (x == mappy -> mapData [TileTopY][TileX] - 1) {
            topTile = true;
        }
    }
    
 
    // if tile below is solid, reset bottom to be on top
    if (bottomTile) {
        float worldBotY = -1 * TILE_SIZE * TileBottomY;
        if (worldBotY > player.position.y - player.sizeEnt.y/2) {
            player.position.y += (worldBotY - player.position.y - player.sizeEnt.y/2) + TILE_SIZE;
        }
        player.collidedBottom = true;
        player.velocity.y = 0.0f;
    }
    
    
    // if tile above is solid
    if (topTile) {
        float worldTopY = -1 * TILE_SIZE * TileTopY;
        if (worldTopY - TILE_SIZE < player.position.y + player.sizeEnt.y/2) {
            player.position.y -= (player.position.y + player.sizeEnt.y/2 - worldTopY) + TILE_SIZE;
        }
        player.collidedTop = true;
        player.velocity.y = 0.0f;
    }
  
}

void GameState::Reset () {
    lives = 5;
    level = 0;
    LoadLevel();
}


