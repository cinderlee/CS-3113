//
//  ParticleEmitter.hpp
//  NYUCodebase
//
//  Created by Cindy Lee on 5/29/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef ParticleEmitter_hpp
#define ParticleEmitter_hpp

#include <stdio.h>
#include <vector>
#include "Vector3.h"
#include "ShaderProgram.h"
#include "Matrix.h"


class Particle {
public:
    Vector3 position = Vector3(0.0, 0.0, 0.0);
    Vector3 velocity = Vector3 (0.0, 0.0f, 0.0);
    float lifetime;
};

class ParticleEmitter {
public:
    
    ParticleEmitter( int textureID, unsigned int particleCount, float x, float y);
    ParticleEmitter();
    ~ParticleEmitter();
    
    float lerp(float v0, float v1, float t);
    void Update(float elapsed);
    void Render(ShaderProgram* program);
    Vector3 position;
    Vector3 gravity = Vector3 (0.0, -8.0, 0.0);
    float maxLifetime;
    std::vector <Particle> particles;
    Vector3 velocity = Vector3 (2.0, -1.0, 0.0);
    Vector3 deviation = Vector3 (2.0, 5.0, 0.0);
    
    int textureID;
    float startSize = 0.25f;
    float endSize = 0.0f;
};

#endif /* ParticleEmitter_hpp */
