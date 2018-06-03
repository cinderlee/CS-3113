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
#include "ShaderProgram.h"
#include <vector>
#include <string>
#include <math.h>
#define TILE_SIZE 0.3f

GameState::GameState () {}

float GameState::lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

// initializing player and enemies
void GameState::Initiate(int spriteTiles, int spritesterPlayer, int spritesterEnemy, int textureID){
    sprites = spriteTiles;
    spritePlayer = spritesterPlayer;
    spriteEnemy = spritesterEnemy;
    partTexture = textureID;
    partSystem = ParticleEmitter (partTexture, 30, 0.0f, 0.0f );
    LoadLevel();
}

// load map and entities of the map
void GameState::LoadLevel () {
    if (mappy != nullptr) {
        delete mappy;
    }
    mappy = new FlareMap();
    enemies.clear ();
    platforms.clear ();
    powerUp.clear ();
    std::stringstream stream;
    stream << "NYUCodebase.app/Contents/Resources/Resources/FinalLevel" << level << ".txt";
    mappy -> Load (stream.str());
    for (size_t index = 0; index < mappy -> entities.size (); index++ ){
        //create the player
        if (mappy -> entities [index].type == "playerRed" && ( level == 0 || level == 1 || level == 4) ){
        
            if (level == 4  && lives == 0) {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 297/1024.0f, 39/1024.0f, 46/1024.0f, TILE_SIZE, TILE_SIZE);
            }
            if (level == 4 && lives != 0) {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 650/1024.0f, 685/1024.0f, 56/1024.0f, 38/1024.0f, TILE_SIZE, TILE_SIZE);
            }
            else {
                player = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 850/1024.0f, 518/1024.0f, 39/1024.0f, 48/1024.0f, TILE_SIZE, TILE_SIZE);
                player.gravity.y = -2.5f;
                player.type = "playerRed";
            }
        }
        
        if (level != 1) {
            if (mappy -> entities [index].type == "playerBlue" ) {
                player.position = Vector3 ((mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f);
            }
            if (mappy -> entities [index].type == "playerGreen") {
                player.position = Vector3((mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f);
                player.gravity.y = -3.5f;
            }
        }
        
        // creating the enemies - categorized into enemyGround, enemyAir, enemyShooter
        
        // walking enemy - enemyGround
        if (mappy -> entities [index].type == "enemyWalking") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 928.0f/1024.0f, 949/1024.0f, 32/1024.0f, 44/1024.0f, TILE_SIZE, TILE_SIZE));
            //928"    y="949"    width="32"    height="44
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies[enemies.size () - 1].type = "enemyGround";
        }
        
        // spider enemy - enemyGround
        if (mappy -> entities [index].type == "spider") {
            enemies.push_back (Entity (spriteEnemy, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 0.0f, 326/512.0f, 71/1024.0f, 45/512.0f, TILE_SIZE * 2, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.5f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies[enemies.size () - 1].type = "enemyGround";
        }
        
        // flying enemy - enemyAir
        if (mappy -> entities [index].type == "enemyFlying") {
            enemies.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 706.0f/1024.0f, 839/1024.0f, 52/1024.0f, 36/1024.0f, TILE_SIZE, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.1f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies [enemies.size () - 1].type = "enemyAir";
        }
        
        // ghost enemy - enemyAir
        if (mappy -> entities [index].type == "ghost") {
            enemies.push_back (Entity (spriteEnemy, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 528.0f/1024.0f, 147/512.0f, 51/1024.0f, 73/512.0f, 2* TILE_SIZE, TILE_SIZE));
            enemies [enemies.size () - 1].velocity.x = 0.1f;
            enemies [enemies.size () - 1].gravity.y = -1.0f;
            enemies[enemies.size () - 1].type = "ghost";
        }
        
        
        // keys, needed to unlock next level 
        if (mappy -> entities [index].type == "keyRed") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 962/1024.0f, 252/1024.0f, 29/1024.0f, 30/1024.0f, TILE_SIZE /2, TILE_SIZE/2 );
            key.type = "keyRed";
        }
        
        if (mappy -> entities [index].type == "keyGreen") {
            key = Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961/1024.0f, 495/1024.0f, 29/1024.0f, 30/1024.0f, TILE_SIZE /2, TILE_SIZE/2);
            key.type = "keyGreen";
        }
        
        if (mappy -> entities [index].type == "plantRed") {
            powerUp.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961/1024.0f, 949/1024.0f, 29/1024.0f, 55/1024.0f, TILE_SIZE, TILE_SIZE));
            powerUp [powerUp.size () - 1].type = "plantRed";
            //partSystem.ResetLocations(powerUp.position.x, powerUp.position.y);
        }
        
        if (mappy -> entities [index].type == "plantBlue") {
            powerUp.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 961/1024.0f, 949/1024.0f, 29/1024.0f, 55/1024.0f, TILE_SIZE, TILE_SIZE));
            powerUp [powerUp.size () - 1].type = "plantBlue";
            //partSystem.ResetLocations(powerUp.position.x, powerUp.position.y);
        }
        
        if (mappy -> entities [index].type == "plantGreen") {
            powerUp.push_back (Entity (spritePlayer, (mappy -> entities[index].x + 0.5f) * TILE_SIZE, (mappy -> entities[index].y + 0.5f) * -1 * TILE_SIZE, 0.0f, 963/1024.0f, 194/1024.0f, 26/1024.0f, 50/1024.0f, TILE_SIZE, TILE_SIZE));
            //963"    y="194"    width="26"    height="50
            powerUp[powerUp.size () - 1].type = "plantGreen";
            //partSystem.ResetLocations(powerUp.position.x, powerUp.position.y);
        }
        
        if (mappy -> entities [index].type == "platformH") {
//            platforms.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE - 3.0 * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 195.0f/1024.0f, 130/1024.0f, 64/1024.0f, 64/1024.0f, TILE_SIZE, TILE_SIZE));
//            platforms [platforms.size () - 1].velocity = Vector3 ( 1.0f, 0.0f, 0.0f);
//            platforms [platforms.size () - 1].type = "platformHEnd";
            platforms.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 195.0f/1024.0f, 65/1024.0f, 64/1024.0f, 64/1024.0f, TILE_SIZE * 6.5, TILE_SIZE));
            platforms [platforms.size () - 1].velocity = Vector3 ( 1.0f, 0.0f, 0.0f);
            platforms [platforms.size () - 1].type = "platformH";
            //platforms [platforms.size () - 1].direction = -1;
        }
        if (mappy -> entities [index].type == "platformV") {
//            platforms.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE - 3.5 * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 195.0f/1024.0f, 130/1024.0f, 64/1024.0f, 64/1024.0f, TILE_SIZE, TILE_SIZE));
//            platforms [platforms.size () - 1].velocity = Vector3 ( 0.0f, 1.0f, 0.0f);
//            platforms [platforms.size () - 1].type = "platformV";
            platforms.push_back (Entity (spritePlayer, (mappy -> entities [index].x + 0.5) * TILE_SIZE, (mappy -> entities[index].y + 0.5) * -1 * TILE_SIZE, 0.0f, 195.0f/1024.0f, 65/1024.0f, 64/1024.0f, 64/1024.0f, TILE_SIZE * 5, TILE_SIZE));
            platforms [platforms.size () - 1].velocity = Vector3 ( 0.0f, 1.0f, 0.0f);
            platforms [platforms.size () - 1].type = "platformV";
        }
    }
}

// update enemy movements
void GameState::UpdateEnemyMovement(float elapsed) {
    for (int index = 0; index < enemies.size (); index++) {
    
        // Ground AIs
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
        
        // Air AIs
        if (enemies [index].type == "enemyAir" || enemies [index].type == "ghost" ) {
            if (enemies [index].DistanceTo(&player) <= 1.0f) {
                float x = enemies [index].DistanceToX (&player);
                float y = enemies [index].DistanceToY (&player);
                
                //adjust velocities based on value of distances
                if ( x < 0 ) {
                    enemies [index].velocity.x = 0.5f;
                }
                else if (x > 0){
                    enemies [index].velocity.x = -0.5f;
                }
                else {
                    enemies [index].velocity.x = 0.0f;
                }
                if ( y < 0 ) {
                    enemies [index].velocity.y = 0.5f;
                }
                else if ( y > 0) {
                    enemies [index].velocity.y = -0.5f;
                }
                else {
                    enemies [index].velocity.y = 0.0f;
                }
            }
            
            // if player not in range, don't move
            else {
                enemies [index].velocity.x = 0.0f;
                enemies [index].velocity.y = 0.0f;
            }
            enemies [index].position.x += enemies [index].velocity.x * elapsed;
            enemies [index].position.y += enemies [index].velocity.y * elapsed;
        }
    }
    
    
    // check for collision between enemy entities
    for (int index = 0; index < enemies.size (); index++) {
        for (int index2 = 0; index2 < enemies.size (); index2++ ){
            if (index != index2 && enemies [index].active && enemies [index2].active) {
                if (enemies [index].Collision (&enemies[index2])) {
                    enemies [index].velocity.x *= -1;
                }
            }
        }
    
    }
    
    for (int i = 0; i < playerBullets.size (); i++) {
        playerBullets[i].position.x += playerBullets [i].velocity.x * elapsed;
        playerBullets[i].distance += playerBullets [i].velocity.x * elapsed;
    }
    
    
    
    for (int index= 0 ; index < platforms.size () ; index ++ ) {
        
        if (player.CollisionPlatformY(&platforms [index])
            //&& player.position.y - player.sizeEnt.y/2 == platforms[index].position.y + platforms[index].sizeEnt.y / 2
            ) {
            player.position.x += platforms[index].velocity.x * elapsed;
            player.position.y += platforms[index].velocity.y * elapsed;
            //player.collidedPlatform = false;
        }
        
        platforms [index].position.x += platforms[index].velocity.x * elapsed;
        platforms [index].position.y += platforms[index].velocity.y * elapsed;
        if (platforms [index].velocity.x < 0 || platforms [index].velocity.y < 0 ) {
            platforms [index].distance += -platforms [index].velocity.x * elapsed + -platforms [index].velocity.y * elapsed;
        }
        else {
            platforms [index].distance += platforms [index].velocity.x * elapsed + platforms [index].velocity.y * elapsed;
        }
        
        if (platforms [index].distance >= 3.0) {
            platforms [index].distance = 0.0f;
            platforms [index].velocity.x *= -1;
            platforms [index].velocity.y *= -1;
        }
    }
    
    for (int i = 0; i < powerUp.size (); i++) {
        if (powerUpObtained && powerUp[i].type == "plantObtained" && powerUp[i].DistanceToY(&player) < 0.6f) {
            powerUp [i].position.y += 0.5 * elapsed;
        }
        if (powerUpObtained && powerUp [i].DistanceToY(&player) >= 0.6f && player.active && powerUp [i].type == "plantObtained") {
            player.active = false;
            partSystem.ResetLocations(powerUp [i].position.x, powerUp [i].position.y);
            if (powerUp [i].DistanceToX(&player) > 0 && partSystem.velocity.x > 0) {
                partSystem.velocity.x *= -1;
            }
            if (powerUp [i].DistanceToX(&player) < 0 && partSystem.velocity.x < 0) {
                partSystem.velocity.x *= -1;
            }
        }
        
        if (!powerUpObtained && powerUp [i].type == "plantObtained") {
            if (player.type == "playerRed") {
                player = Entity (spritePlayer, player.position.x, player.position.y, 0.0f, 762/1024.0f, 203/1024.0f, 45/1024.0f, 54/1024.0f, TILE_SIZE, TILE_SIZE);
                player.type = "playerBlue";
                player.gravity.y = -3.5f;
                powerUp [i].type = "plantDead";
                break;
            }
            
            if (player.type == "playerBlue") {
                player = Entity (spritePlayer, player.position.x, player.position.y, 0.0f, 890/1024.0f, 0/1024.0f, 38/1024.0f, 50/1024.0f, TILE_SIZE, TILE_SIZE);
                player.type = "playerGreen";
                player.gravity.y = -3.5f;
                powerUp [i].type = "plantDead";
                break;
            }
            
            if (player.type == "playerGreen") {
                player = Entity (spritePlayer, player.position.x, player.position.y, 0.0f, 850/1024.0f, 518/1024.0f, 39/1024.0f, 48/1024.0f, TILE_SIZE, TILE_SIZE);
                player.gravity.y = -3.5f;
                player.type = "playerRed";
                powerUp [i].type = "plantDead";
                break;
            }
        }
    }
    
    
}

// update when player can move on to next level
void GameState::UpdateLevel() {
    
    level ++;
    keyObtained = false;
    
    LoadLevel();
}


// return the game level
int GameState::GetLevel(){
    return level;
}

// return the number of lives left
int GameState::GetLives (){
    return lives;
}

// drawing game
void GameState::Draw (ShaderProgram* program) {
    mappy -> Draw (program, sprites);
    player.Draw (program);
    key.Draw (program);
    for (int i = 0; i < powerUp.size (); i++) {
        if (powerUp[i].type != "plantDead") {
            powerUp [i].Draw (program);
        }
    }
    for (int i = 0; i < platforms.size (); i++) {
        platforms [i].Draw (program);
    }
    for (int i = 0; i < enemies.size (); i++){
        if (enemies [i].active) {
            enemies[i].Draw (program);
        }
    }
    for (int i = 0; i < playerBullets.size() ; i++ ) {
        playerBullets [i].Draw (program);
    }
}


bool GameState::shouldRemoveBullet (Entity thing) {
    return (! (thing.active)  || thing.distance >= 1.0f );
}

// checking for any collisions in game between entities
void GameState::CollisionEntities () {
    
    // if collision between enemy and player
    // reset player to start of the game
    for (int index = 0; index < enemies.size (); index++) {
        if (enemies [index].active && player.Collision(&enemies[index]) ) {
            lives--;
            keyObtained = false;
            if (!lives ) {
                level = 4;
                nextLevel = true;
            }
            else {
                level = 1;
                LoadLevel();
            }
        }
    }
    
    for (size_t index = 0 ; index < playerBullets.size () ; index++) {
        for (int enemy = 0; enemy < enemies.size (); enemy++) {
            
            // collision between enemy and player bullet
            if ( enemies [enemy].type != "ghost" && (level == 1 && player.type == "playerBlue") || (level == 2 && player.type == "playerGreen") || (level == 3 && player.type == "playerRed") ) {
                if (playerBullets [index].Collision(& (enemies[enemy])) && enemies[enemy].active && enemies [enemy].active && playerBullets[index].active) {
                    enemies[enemy].active = false;
                    
                    playerBullets [index].active = false;
                    break;
                }
            }
        }
    }
    
    // removing bullets from vector
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), &GameState::shouldRemoveBullet), playerBullets.end());
            
    
    if (player.Collision (&key)) {
        keyObtained = true; 
    }
    
    for (int i = 0; i < powerUp.size (); i++) {
        if (player.Collision (&powerUp [i]) && powerUp[i].type != "plantDead" &&
            ((player.type == "playerRed" && powerUp[i].type != "plantRed") ||
             (player.type == "playerBlue" && powerUp [i].type != "plantBlue") ||
             (player.type == "playerGreen" && powerUp [i].type != "plantGreen"))) {
                powerUpObtained = true;
                powerUp [i].type = "plantObtained";
                break;
        }
    }
    
    // move on to next level if at the door with key
    int TileX;
    int TileY;
    player.worldToTileCoordinates(player.position.x, player.position.y, &TileX, &TileY);
    if (keyObtained && (mappy -> mapData [TileY] [TileX] - 1 == 152 || mappy -> mapData [TileY] [TileX] - 1 == 153)) {
        //UpdateLevel ();
        nextLevel = true;
    }
//    for (int i = 0; i < platforms.size (); i++ ) {
//        player.CollisionPlatformY(&platforms [i]) ;
//    }
    
    int TileBottom;
    player.worldToTileCoordinates(player.position.x, player.position.y - player.sizeEnt.y/2, &TileX, &TileBottom);
    if (mappy -> mapData [TileBottom] [TileX] - 1 == 234 || mappy -> mapData [TileBottom] [TileX] - 1 == 232) {
        lives --;
        keyObtained = false;
        if (!lives) {
            level = 4;
            nextLevel = true;
        }
        else {
            level = 1;
        }
        //nextLevel = true;
        LoadLevel();
    }
}

// check and resolve horizontal collision
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
    
//    for (int i = 0; i < platforms.size (); i++ ) {
//        player.CollisionPlatformX (&platforms[i]);
//    }
    
    
}

// check and adjust vertical collision
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
    
    for (int i =0 ; i < platforms.size (); i++ ) {
        if (player.CollisionPlatformY(&platforms[i])) {
            if (player.position.y - player.sizeEnt.y/2 <= platforms[i].position.y + platforms[i].sizeEnt.y / 2 &&
                player.position.y + player.sizeEnt.y/2 > platforms[i].position.y + platforms[i].sizeEnt.y/2 ) {
                player.position.y = platforms[i].position.y + platforms[i].sizeEnt.y/2 + player.sizeEnt.y/2;
                player.collidedBottom = true;
                player.collidedPlatform = true;
                
                player.velocity.y = 0.0f;
            }
            if (player.position.y + player.sizeEnt.y/2 >= platforms[i].position.y - platforms[i].sizeEnt.y/2 &&  player.position.y != platforms[i].position.y + platforms[i].sizeEnt.y/2 + player.sizeEnt.y/2) {
                player.position.y = platforms[i].position.y - platforms[i].sizeEnt.y/2 - player.sizeEnt.y/2 ;
                player.collidedTop = true;
                player.velocity.y = -1.0f;
            }
        }
    }
}

// shooting a bullet from player
void GameState::shootPlayerBullet(int sprites) {
    Entity playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 858.0f/1024.0f, 230.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.3f, 0.3f);
    if (player.type == "playerBlue") {
        playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 856.0f/1024.0f, 421.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.3f, 0.3f);
    }
    if (player.type == "playerGreen") {
        playerBullet = Entity (sprites, 0.0f, -1.5f, 0.0f, 849/1024.0f, 310/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.3f, 0.3f);
    }
    playerBullet.active = true;
    playerBullet.position.x = player.position.x + player.sizeEnt.x;
    playerBullet.position.y = player.position.y;
    playerBullet.velocity.x = 1.0f;
    playerBullet.distance = 0.0f;
    playerBullet.rotation = M_PI/2;
    playerBullets.push_back(playerBullet);
}

// reset game state
void GameState::Reset () {
    lives = 5;
    level = 0;
    LoadLevel();
}


