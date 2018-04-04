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

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;
Matrix projectionMatrix;
Matrix viewMatrix;
#define FIXED_TIMESTEP 0.0166666f

class Shape {
public:
    Shape (float width, float height) :width (width), height (height) {}
    virtual std::vector<Vector3>& cornerVertices () = 0;
    virtual std::vector <float>& triVertices () = 0;
    std::vector <float> vertices;
    std::vector <Vector3> corner;
    float width;
    float height;
};

class Rectangle: public Shape{
public:
    Rectangle (float width, float height): Shape (width, height)  {
        float x = 0.0;
        float y = 0.0;
        float verticesUN [] = {x - width/2, y + height/2, x -width/2, y - height/2, x + width/2, y -height/2, x - width/2, y + height/2, x + width/2, y - height/2, x + width/2, y + height/2};
        for (int i = 0; i < 12; i++){
            vertices.push_back (verticesUN [i]);
        }
        for (int i = 0; i < 6; i+=2) {
            corner.push_back ( Vector3 (vertices [i], vertices[i+1], 0.0f) );
        }
        corner.push_back ( Vector3 (vertices [10], vertices [11], 0.0f));
    }
    
    std::vector<Vector3>& cornerVertices () {
        
        return corner;
    }
    
    std::vector <float>& triVertices () {
        return vertices;
    }
    
};

class Entity {
public:
    Entity (Rectangle itemster) : item (itemster) {
    }
    
    void Draw (ShaderProgram *program) {
        
        glUseProgram((*program).programID);
        modelMatrix.Identity();
        modelMatrix.Translate(objectPosition.x, objectPosition.y, 0.0f);
        modelMatrix.Rotate(rotator);
        modelMatrix.Scale (scale.x, scale.y, 1.0f);
        program ->SetModelMatrix(modelMatrix);
        glVertexAttribPointer(program -> positionAttribute, 2, GL_FLOAT, false, 0, item.triVertices().data ());
        glEnableVertexAttribArray(program -> positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->positionAttribute);
        
    }
    
    Rectangle item;
    Vector3 velocity = Vector3(1.0f, 1.0f, 0.0f);
    Matrix modelMatrix;
    
    float rotator = 0.0f;
    Vector3 scale = Vector3 (1.0f, 1.0f, 1.0f);
    Vector3 objectPosition = Vector3 (0, 0, 0);
    
    bool topBar = false;
    bool bottomBar = false;
    bool leftBar = false;
    bool rightBar = false;
    
};


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 960, 540);
    srand (time (NULL));
    
    
    projectionMatrix.SetOrthoProjection(-5.25, 5.25, -3.0f, 3.0f, -1.50f, 1.5f);
    ShaderProgram program;
    program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    program.SetViewMatrix(viewMatrix);
    program.SetProjectionMatrix(projectionMatrix);
    
    float randomplacer = -1.0f;
    
    std::vector <Entity> cornerShapes;
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
    left.objectPosition.x  = -5.25f-1.5f;
    right.objectPosition.x = 5.25f + 1.5f;
    top.objectPosition.y = 4.5f;
    bottom.objectPosition.y = -4.5f;
    cornerShapes.push_back (top);
    cornerShapes.push_back (bottom);
    cornerShapes.push_back (left);
    cornerShapes.push_back (right);
    

    std::vector <Entity> shapes;
    for (int i = 0; i < 5; i++) {
        Rectangle rect = Rectangle (4.0, 4.0);
        Entity one = Entity (rect);
        one.scale.x = 1.0f / (rand () % 4 + 1 );
        std :: cout << one.scale.x << std::endl;
        one.scale.y = 1.0f / (rand () % 4 + 1 ) ;
        std :: cout << one.scale.y << std::endl;
        one.rotator = 1 + 1.5 * (rand () % 100/ 50.0f);
        float translatorx = 1.0f / rand ();
        float translatory = 1.0f / rand () ;
        
        one.objectPosition.x += translatorx * randomplacer;
        one.objectPosition.y += translatory * randomplacer;
        
        std:: cout << one.objectPosition.x  << " " << one.objectPosition.y << std::endl;
        randomplacer *= -1.0f;
        shapes.push_back (one);
    }
    
    std :: cout << -1 * 0;
    for (int i = 0; i < 1; i++) {
         std :: cout << shapes[i].objectPosition.x;
    }
    SDL_Event event;
    bool done = false;
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        program.SetColor(0.0f, 1.0f,0.0f , 0.0f);
        //std::cout << shapes.size ()<< std::endl;
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue; }
        while(elapsed >= FIXED_TIMESTEP) {
            
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
//                        std:: cout << penetration.first<< std::endl;
//                        std::cout << penetration.second << std:: endl;
                        if (cornerShapes[j].topBar) {
                            shapes[i].objectPosition.x -= -1* penetration.first;
                            shapes [i].objectPosition.y -= -1* penetration.second;
                            
                            shapes[i].velocity.y *= -1;
                            
//                            std::cout << "TOP" << std::endl;
                        }
                        if (cornerShapes[j].bottomBar ) {
                            shapes[i].objectPosition.x -= -1* penetration.first;
                            shapes [i].objectPosition.y -= -1* penetration.second;
                            
                            shapes[i].velocity.y *= -1;
//                            std::cout << "BOTTOm" << std::endl;
                        }
                        if (cornerShapes[j].leftBar) {
                            shapes[i].objectPosition.x -= -1* penetration.first;
                            shapes [i].objectPosition.y -= -1* penetration.second;
                            
                            shapes[i].velocity.x *= -1;
//                             std::cout << "LEFT" << std::endl;
                        }
                        if (cornerShapes[j].rightBar) {
                            shapes[i].objectPosition.x -= -1* penetration.first;
                            shapes [i].objectPosition.y -= -1* penetration.second;
                            
                            shapes[i].velocity.x *= -1;
//                             std::cout << "RIGHT" << std::endl;
                        }
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
                        shapes [i].velocity.x *= -1;
                        shapes [i].velocity.y *= -1;
                        shapes [j].velocity.x *= -1;
                        shapes [j].velocity.y *= -1;
                    }
                }
            }
            for (int i =0; i < cornerShapes.size (); i ++) {
                cornerShapes[ i]. Draw (&program);
            }
            
            for (int i = 0; i < shapes.size (); i ++ ) {
                //shapes[i].objectPosition.x += shapes[i].velocity.x * FIXED_TIMESTEP;
                //shapes [i].objectPosition.y += shapes[i].velocity.y * FIXED_TIMESTEP;
                shapes [i].Draw (&program);
            }
        
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
