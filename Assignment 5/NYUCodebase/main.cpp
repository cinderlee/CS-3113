#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "SatCollision.h"
#include "Vector3.h"
#include <time.h>
#include "Shape.h"
#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;
Matrix projectionMatrix;
Matrix viewMatrix;
ShaderProgram program;
std::vector <Entity> cornerShapes;
 std::vector <Entity> shapes;

#define FIXED_TIMESTEP 0.0166666f

void Setup();
void ProcessEvents (SDL_Event& event, bool& done);
void Update ();
void Render ();

int main(int argc, char *argv[])
{
    srand (time (NULL));
    Setup();
    
    SDL_Event event;
    bool done = false;
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    

    while (!done) {
        ProcessEvents (event, done);
        glClear(GL_COLOR_BUFFER_BIT);
        //program.SetColor(0.0f, 1.0f,0.0f , 0.0f);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue; }
        while(elapsed >= FIXED_TIMESTEP) {
            Update ();
            
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        Render ();
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}

void Setup () {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 960, 540);
    
    program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    

    
    glUseProgram(program.programID);
    
    // set view and projection matrices
    projectionMatrix.SetOrthoProjection(-5.25, 5.25, -3.0f, 3.0f, -1.50f, 1.5f);
    program.SetViewMatrix(viewMatrix);
    program.SetProjectionMatrix(projectionMatrix);
    
    
    Rectangle topster = Rectangle (10.50, 3.0f);
    Rectangle bottomster = Rectangle (10.50, 3.0f);
    Rectangle leftster = Rectangle (3.0f, 6.0f);
    Rectangle rightster = Rectangle (3.0f, 6.0f);
    Entity left = Entity (leftster);
    left.leftBar = true;
    Entity right = Entity (rightster);
    right.rightBar = true;
    Entity top = Entity (topster);
    top.topBar = true;
    Entity bottom = Entity (bottomster);
    bottom.bottomBar = true;
    left.objectPosition.x  = -5.25f - 1.5f;
    right.objectPosition.x = 5.25f + 1.5f;
    top.objectPosition.y = 3.0 + 1.5f;
    bottom.objectPosition.y = -3.0f - 1.5f;
    
    left.modelMatrix.Translate(-5.25f - 1.5f, 0.0f, 0.0f);
    right.modelMatrix.Translate(5.25f + 1.5f, 0.0f, 0.0f);
    top.modelMatrix.Translate(0.0f, 3.0f + 1.5f, 0.0f);
    bottom.modelMatrix.Translate(0.0f, -3.0f - 1.5f, 0.0f);
    
    cornerShapes.push_back (top);
    cornerShapes.push_back (bottom);
    cornerShapes.push_back (left);
    cornerShapes.push_back (right);
    
   
    float translatorx = 0.0f;
    float translatory = 0.0f;
   
    for (int i = 0; i < 7 ; i ++) {
        Rectangle rect = Rectangle (1.0f, 1.0f);
        Entity one = Entity (rect);
       
        one.scale.x = (rand() / (float)RAND_MAX * 0.4) + 0.5f;
        std :: cout << one.scale.x << std::endl;
        
        one.scale.y = (rand() / (float)RAND_MAX * 0.4) + 0.5f;
        
        one.rotator = rand() / (2 * M_PI);
        translatorx = 1.0f / (rand () + 1);
        translatory = 1.0f  / (rand () + 1);
        
        one.red = 1.0f / (rand () + 1);
        one.blue = 1.0f / (rand () + 1);
        one.green = 1.0f / (rand () + 1);
        
        one.modelMatrix.Translate(translatorx, translatory, 0.0f);
        one.modelMatrix.Rotate (one.rotator);
        one.modelMatrix.Scale (one.scale.x, one.scale.y, 0.0f);
        
        shapes.push_back (one);
        
    
    }
    
}

void ProcessEvents (SDL_Event& event, bool& done) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}

void Update () {
    for (int i = 0; i < shapes.size (); i ++ ) {
        for (int j = 0; j < cornerShapes.size (); j++ ) {
            std::pair<float,float> penetration;
            
            std::vector<std::pair<float,float>> e1Points;
            std::vector<std::pair<float,float>> e2Points;
            
            for(int s=0; s < shapes[i].item.cornerVertices().size(); s++) {
                Vector3 point = shapes[i].modelMatrix * shapes[i].item.cornerVertices ()[s];
                e1Points.push_back(std::make_pair(point.x, point.y));
                
            }
            
            for(int s=0; s < cornerShapes [j].item.cornerVertices().size (); s++) {
                Vector3 point = cornerShapes[j].modelMatrix * cornerShapes[j].item.cornerVertices()[s];
                e2Points.push_back(std::make_pair(point.x, point.y));
                
            }
            
            bool collided = CheckSATCollision(e1Points, e2Points, penetration);
            
            
            if (collided) {
                std::cout << "YOOOO" << std::endl;
                std:: cout << penetration.first<< std::endl;
                std::cout << penetration.second << std:: endl;
                if (cornerShapes[j].topBar) {
                    
                    shapes[i].objectPosition.x += penetration.first;
                    shapes [i].objectPosition.y += penetration.second;
                    
                    shapes[i].velocity.y *= -1;
                    
                    std::cout << "TOP" << std::endl;
                }
                else if (cornerShapes[j].bottomBar ) {
                    shapes[i].objectPosition.x += penetration.first;
                    shapes [i].objectPosition.y += penetration.second;
                    
                    shapes[i].velocity.y *= -1;
                    std::cout << "BOTTOm" << std::endl;
                }
                else if (cornerShapes[j].leftBar) {
                    shapes[i].objectPosition.x +=  penetration.first;
                    shapes [i].objectPosition.y += penetration.second;
                    
                    shapes[i].velocity.x *= -1;
                    std::cout << "LEFT" << std::endl;
                }
                else if (cornerShapes[j].rightBar) {
                    shapes[i].objectPosition.x +=  penetration.first;
                    shapes [i].objectPosition.y +=  penetration.second;
                    
                    shapes[i].velocity.x *= -1;
                    std::cout << "RIGHT" << std::endl;
                }
                
                shapes[i].modelMatrix.Translate(penetration.first, penetration.second, 0.0f);
            }
        }
    }
    
    for (int i = 0 ; i < shapes.size (); i ++) {
        for (int j = i + 1 ; j < shapes.size (); j++ ) {
            
            std::pair<float,float> penetration;
            
            std::vector<std::pair<float,float>> e1Points;
            std::vector<std::pair<float,float>> e2Points;
            
            for(int s = 0; s < shapes[i].item.cornerVertices().size(); s++) {
                Vector3 point = shapes[i].modelMatrix * shapes[i].item.cornerVertices ()[s];
                e1Points.push_back(std::make_pair(point.x, point.y));
            }
            
            for(int s = 0; s < shapes [j].item.cornerVertices().size (); s++) {
                Vector3 point = shapes[j].modelMatrix * shapes[j].item.cornerVertices()[s];
                e2Points.push_back(std::make_pair(point.x, point.y));
            }
            
            bool collided = CheckSATCollision(e1Points, e2Points, penetration);
            if (collided) {
                
                shapes[i].objectPosition.x += (penetration.first * 0.5f);
                shapes[i].objectPosition.y += (penetration.second * 0.5f);
                
                
                shapes[j].objectPosition.x -= (penetration.first * 0.5f);
                shapes [j].objectPosition.y -= (penetration.second * 0.5f);
                
                shapes[i].modelMatrix.Translate(penetration.first * 0.5f,penetration.second * 0.5f, 0.0f);
                shapes[j].modelMatrix.Translate(-1* penetration.first * 0.5f, -1* penetration.second * 0.5f, 0.0f);
                shapes [i].velocity.x *= -1;
                shapes [i].velocity.y *= -1;
                shapes [j].velocity.x *= -1;
                shapes [j].velocity.y *= -1;
            }
        }
    }
    
    for (int i = 0; i < shapes.size () ; i++) {
        shapes [i].objectPosition.x += shapes[i].velocity.x  * FIXED_TIMESTEP;
        shapes [i].objectPosition.y += shapes[i].velocity.y  * FIXED_TIMESTEP;
        shapes[i].modelMatrix.Translate(shapes[i].velocity.x * FIXED_TIMESTEP, shapes[i].velocity.y * FIXED_TIMESTEP, 0.0f);
    }

}

void Render () {
    for (int i =0; i < cornerShapes.size (); i ++) {
        cornerShapes[ i]. Draw (&program);
    }
    
    for (int i = 0; i < shapes.size (); i ++ ) {
        
        shapes [i].Draw (&program);
    }
}
