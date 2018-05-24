
#include "FlareMap.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include "ShaderProgram.h"

FlareMap::FlareMap() {
	mapData = nullptr;
	mapWidth = -1;
	mapHeight = -1;
}

FlareMap::~FlareMap() {
	for(int i=0; i < mapHeight; i++) {
		delete mapData[i];
	}
	delete mapData;
}

bool FlareMap::ReadHeader(std::ifstream &stream) {
	std::string line;
	mapWidth = -1;
	mapHeight = -1;
	while(std::getline(stream, line)) {
		if(line == "") { break; }
		std::istringstream sStream(line);
		std::string key,value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if(key == "width") {
			mapWidth = std::atoi(value.c_str());
		} else if(key == "height"){
			mapHeight = std::atoi(value.c_str());
		}
	}
	if(mapWidth == -1 || mapHeight == -1) {
		return false;
	} else {
		mapData = new unsigned int*[mapHeight];
		for(int i = 0; i < mapHeight; ++i) {
			mapData[i] = new unsigned int[mapWidth];
		}
		return true;
	}
}

bool FlareMap::ReadLayerData(std::ifstream &stream) {
	std::string line;
	while(getline(stream, line)) {
		if(line == "") { break; }
		std::istringstream sStream(line);
		std::string key,value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if(key == "data") {
			for(int y=0; y < mapHeight; y++) {
				getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for(int x=0; x < mapWidth; x++) {
					std::getline(lineStream, tile, ',');
					unsigned char val =  (unsigned char)atoi(tile.c_str());
                    mapData [y][x] = val;
//                    if(val > 0) {
//                        mapData[y][x] = val-1;
//                    } else {
//                        mapData[y][x] = 0;
//                    }
				}
			}
		}
	}
	return true;
}


bool FlareMap::ReadEntityData(std::ifstream &stream) {
	std::string line;
	std::string type;
	while(getline(stream, line)) {
		if(line == "") { break; }
		std::istringstream sStream(line);
		std::string key,value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if(key == "type") {
			type = value;
		} else if(key == "location") {
			std::istringstream lineStream(value);
			std::string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			
			FlareMapEntity newEntity;
			newEntity.type = type;
			newEntity.x = std::atoi(xPosition.c_str());
			newEntity.y = std::atoi(yPosition.c_str());
			entities.push_back(newEntity);
		}
	}
	return true;
}

void FlareMap::Load(const std::string fileName) {
	std::ifstream infile(fileName);
	if(infile.fail()) {
		assert(false); // unable to open file
	}
	std::string line;
	while (std::getline(infile, line)) {
		if(line == "[header]") {
			if(!ReadHeader(infile)) {
				assert(false); // invalid file data
			}
		} else if(line == "[layer]") {
			ReadLayerData(infile);
		} else if(line == "[Object Layer]") {
			ReadEntityData(infile);
		}
	}
}

//drawing the flare map
void FlareMap::Draw (ShaderProgram *program, int textureID) {
    Matrix modelMatrix;
    glBindTexture(GL_TEXTURE_2D, textureID);
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    int SPRITE_COUNT_X = 22;
    int SPRITE_COUNT_Y = 12;
    float TILE_SIZE = 0.3f;
    for(int y=0; y < mapHeight; y++) {
        for(int x=0; x < mapWidth; x++) {
            if (mapData [y][x] == 0) {
                continue;
            }
            float u = (float)(((int)mapData[y][x] - 1) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
            float v = (float)(((int)mapData[y][x] - 1) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
            float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
            float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
            vertexData.insert(vertexData.end(), {
                TILE_SIZE * x, -TILE_SIZE * y,
                TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
                (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                TILE_SIZE * x, -TILE_SIZE * y,
                (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
            });
            
            texCoordData.insert(texCoordData.end(), {
                u, v,
        u, v+(spriteHeight),
        u+spriteWidth, v+(spriteHeight),
        u, v,
        u+spriteWidth, v+(spriteHeight),
        u+spriteWidth, v
        }); }
    }
    
    program -> SetModelMatrix(modelMatrix);
    glVertexAttribPointer(program -> positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program -> positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data ());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, vertexData.size()/2);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    
}
