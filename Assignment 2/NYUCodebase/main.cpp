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

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Entity {
public:
    Entity (float xcoord, float ycoord, float widthster,
            float heightster, float velx, float vely):
    x (xcoord), y (ycoord), width (widthster), height (heightster),
    velocity_x (velx), velocity_y (vely) {}
    
    void Draw(ShaderProgram &program) {
        glUseProgram(program.programID);
        float verticesUntextured[] = {x - width/2, y + height/2, x -width/2, y - height/2, x + width/2, y -height/2, x - width/2, y + height/2, x + width/2, y - height/2, x + width/2, y + height/2};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesUntextured);
        glEnableVertexAttribArray(program.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);
    }
    
    float x;
    float y;
    float rotation;
    int textureID;
    float width;
    float height;
    float velocity_x;
    float velocity_y;

};


SDL_Window* displayWindow;

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
    ShaderProgram programUntextured;
    programUntextured.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    Matrix modelMatrix;
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    Entity paddle1 = Entity (-3.0, 0.0, 0.15f, 0.5f, 0.0f, 7.0f);
    
    Matrix viewMatrix2;
    Matrix modelMatrix2;
    Entity paddle2 = Entity (3.0, 0.0, 0.15, 0.5f, 0.0f, 7.0f);
    
    Matrix viewMatrix3;
    Matrix modelMatrix3;
    Entity ball = Entity (0.0f, 0.0f, 0.15f, 0.15f, -1.0f, -1.0f);
    
    SDL_Event event;
    bool done = false;
    
    float lastFrameTicks = 0.0f;
    float elapsed = 0.0f;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if(event.type == SDL_KEYDOWN) {
                //if(event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                if (keys [SDL_SCANCODE_DOWN]){
                    if (paddle2.y - paddle2.height/2 - (elapsed * paddle2.velocity_y) < -2.0f ) {
                        paddle2.y = -2.0f + paddle2.height/2;
                    }
                    else {
                        paddle2.y -= elapsed * paddle2.velocity_y;
                    }
                }
                //else if(event.key.keysym.scancode == SDL_SCANCODE_UP) {
                if (keys [SDL_SCANCODE_UP]) {
                    if (paddle2.y + paddle2.height/2 + (elapsed * paddle2.velocity_y) > 2.0f ) {
                        paddle2.y = 2.0f - paddle2.height/2;
                    }
                    else {
                        paddle2.y += elapsed * paddle2.velocity_y;
                    }
                }
                //if(event.key.keysym.scancode == SDL_SCANCODE_S) {
                if (keys [SDL_SCANCODE_S]) {
                    if (paddle1.y - paddle1.height/2 - (elapsed * paddle1.velocity_y) < -2.0f ) {
                        paddle1.y = -2.0f + paddle1.height/2;
                    }
                    else {
                        paddle1.y -= elapsed * paddle1.velocity_y;
                    }
                }
                //else if(event.key.keysym.scancode == SDL_SCANCODE_W) {
                if (keys [SDL_SCANCODE_W]) {
                    if (paddle1.y + paddle1.height/2 + (elapsed * paddle1.velocity_y) > 2.0f ) {
                        paddle1.y = 2.0f - paddle1.height/2;
                    }
                    else {
                        paddle1.y += elapsed * paddle1.velocity_y;
                    }
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
            
        programUntextured.SetModelMatrix(modelMatrix);
        programUntextured.SetViewMatrix(viewMatrix);
        programUntextured.SetProjectionMatrix(projectionMatrix);
        programUntextured.SetColor(1.0, 1.0, 1.0, 0.0f);
        paddle1.Draw (programUntextured);
        
        programUntextured.SetModelMatrix(modelMatrix2);
        programUntextured.SetViewMatrix(viewMatrix2);
        programUntextured.SetColor(1.0, 1.0, 1.0, 0.0);
        paddle2.Draw (programUntextured);
        
        programUntextured.SetModelMatrix(modelMatrix3);
        programUntextured.SetViewMatrix(viewMatrix3);
        programUntextured.SetColor(1.0, 1.0, 1.0, 0.0);
        ball.Draw (programUntextured);
        
        ball.x += elapsed * ball.velocity_x;
        ball.y += elapsed * ball.velocity_y;
        
        if (!(paddle2.y - paddle2.height/2 > ball.y + ball.height / 2) &&
            !( paddle2.y + paddle2.height/2 < ball.y - ball.height/2 ) &&
            !(paddle2.x - paddle2.width/2 > ball.x + ball.width/2) &&
            ! (paddle2.x + paddle2.width/2 < ball.x - ball.width/2 )) {
                ball.velocity_x *= -1;
                std:: cout << "hi";
                
        }
        else if (!(paddle1.y - paddle1.height/2 > ball.y + ball.height / 2) &&
                 !( paddle1.y + paddle1.height/2 < ball.y - ball.height/2 ) &&
                 !(paddle1.x + paddle1.width/2 < ball.x - ball.width/2) &&
                 ! (paddle1.x - paddle1.width/2 > ball.x + ball.width /2)) {
            ball.velocity_x *= -1;
                     std:: cout << "bye";
        }
        else if (ball.y + ball.height/2 >= 2.0f) {
            ball.velocity_y *= -1;
        }
        else if (ball.y - ball.height/2 <= -2.0f) {
            ball.velocity_y *= -1;
        }
        
    
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
