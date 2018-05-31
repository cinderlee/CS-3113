//
//  ParticleEmitter.cpp
//  NYUCodebase
//
//  Created by Cindy Lee on 5/29/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "ParticleEmitter.h"
#include <vector>
#include "Vector3.h"
#include "ShaderProgram.h"
#include "Matrix.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "stb_image.h"

ParticleEmitter::ParticleEmitter(int textureID, unsigned int particleCount, float x, float y): textureID(textureID), position (Vector3 (x, y, 0.0f)){
    
    maxLifetime = 0.5f;
    srand (time (NULL));
    
    particles.resize (particleCount);
    
    for (int i = 0; i < particleCount; i++) {
        particles [i].position.x = position.x;
        particles [i].position.y = position.y;
        particles [i].lifetime = ((float)rand () / (float) RAND_MAX) * maxLifetime;
    }
}

ParticleEmitter:: ParticleEmitter () : position (Vector3 (0.0, 0.0, 0.0)) {}

ParticleEmitter:: ~ParticleEmitter() {

}

void ParticleEmitter::ResetLocations (float x, float y) {
    position.x = x;
    position.y = y;
    for (int i = 0; i < particles.size(); i++) {
        particles [i].position.x = x;
        particles [i].position.y = y;
        particles [i].velocity = Vector3 (0.0f, 0.0f, 0.0f);
    }
}

void ParticleEmitter::Update(float elapsed) {
    for (int i = 0; i < particles.size(); i++ ) {
        particles [i].velocity.y += gravity.y * elapsed;
        particles [i].position.x += particles [i].velocity.x * elapsed;
        particles [i].position.y += particles [i].velocity.y * elapsed;
        particles [i].lifetime += elapsed;
        
        if (particles [i].lifetime >= maxLifetime) {
            particles [i].lifetime -= maxLifetime;
            particles [i].velocity.x = velocity.x;
            particles [i].velocity.y = velocity.y;
            particles [i].position.x = position.x;
            particles [i].position.y = position.y;
            
            if (velocity.x > 0) {
                particles [i].velocity.x += ( (float) rand () / (float) RAND_MAX ) * deviation.x;
            }
            else {
                particles [i].velocity.x += ( (float) rand () / (float) RAND_MAX ) * deviation.x * -1;
            }
            particles [i].velocity.y += ( (float) rand () / (float) RAND_MAX ) * deviation.y;
        }
        
    }
}

float ParticleEmitter::lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void ParticleEmitter::Render(ShaderProgram* program) {
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i=0; i < particles.size(); i++) {
        float m = (particles[i].lifetime/maxLifetime);
        float size = lerp(startSize, endSize, m);
        //float size = 1.0f;
        vertices.insert(vertices.end(), {
            particles[i].position.x - size, particles[i].position.y + size,
            particles[i].position.x - size, particles[i].position.y - size,
            particles[i].position.x + size, particles[i].position.y + size,
            particles[i].position.x + size, particles[i].position.y + size,
            particles[i].position.x - size, particles[i].position.y - size,
            particles[i].position.x + size, particles[i].position.y - size
        });


        texCoords.insert(texCoords.end(), {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        });
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/2);
    
}



