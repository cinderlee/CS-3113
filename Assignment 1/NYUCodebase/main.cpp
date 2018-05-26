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

using namespace std;

SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return retTexture;
}

int main(int argc, char *argv[])
{
    
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 880, 450, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    #ifdef _WINDOWS
        glewInit();
    #endif
    

    glViewport(0, 0, 880, 450);
    ShaderProgram program;
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    GLuint toukenTexture = LoadTexture(RESOURCE_FOLDER"Mikazuki-2.png");
    GLuint articuno = LoadTexture(RESOURCE_FOLDER"Articuno.png");
    GLuint sakura = LoadTexture(RESOURCE_FOLDER"sakura.png");
    GLuint butterfly = LoadTexture(RESOURCE_FOLDER"butterfly.png");
    GLuint background = LoadTexture (RESOURCE_FOLDER"Kanda_Shrine.png");
    
    ShaderProgram programUntextured;
    programUntextured.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    float lastFrameTicks = 0.0f;
    float angle = 0.0f;
    float back = -1.0f;
    float totalTime = 0.0f;
    float move = 0.0f;
    float direction = -1.0f;
    float current_position = 1.5f;
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    // for touken texture
    Matrix modelMatrix;
    modelMatrix.Translate(-1.2f, -1.3f, 0.0f);
    modelMatrix.Rotate (0.50);
    modelMatrix.Scale (0.75f, 0.75f, 1.0f);
   
    // for sakura
    Matrix modelMatrix2;
    modelMatrix2.Translate(-1.75f, -0.55f, 0.0f);
    modelMatrix2.Scale (3.5f, 3.5f, 1.0f);
    
    // for articuno
    Matrix modelMatrix3;
    modelMatrix3.Translate(2.0f, 1.5f, 0.0f);
    
    // for butterfly
    Matrix modelMatrix4;
    modelMatrix4.Translate(2.0f, 1.0f, 0.0f);
    modelMatrix4.Rotate(0.5);
    
    // for background
    Matrix modelBackground;
    modelBackground.Scale (1.80f, 1.5f, 1.0f);
    
    // for untexture polygons
    Matrix shapes;
    Matrix viewMatrixUn;
    shapes.Translate (2.0f, 1.0f, 0.0f);
    
    glUseProgram(program.programID);
    glUseProgram(programUntextured.programID);
    
    SDL_Event event;
    bool done = false;
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        glClearColor(1.0f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        // background set up
        program.SetModelMatrix(modelBackground);
        glBindTexture(GL_TEXTURE_2D, background);
        float vertices3[] = {-2, -1.5, 2, -1.5, 2, 1.5, -2, -1.5, 2, 1.5, -2, 1.5};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
        glEnableVertexAttribArray(program.positionAttribute);
        float texCoords3[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        // sakura tree set up
        program.SetModelMatrix(modelMatrix2);
        glBindTexture(GL_TEXTURE_2D, sakura);
        float vertices2[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
        glEnableVertexAttribArray(program.positionAttribute);
        float texCoords2[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // articuno set up and movement
        program.SetModelMatrix(modelMatrix3);
        glBindTexture(GL_TEXTURE_2D, articuno);
        current_position += elapsed * direction;
        move = elapsed;
        modelMatrix3.SetPosition(current_position, 1.5f, 0.0f);
        // check for bounds
        if (current_position <= -2.55 || current_position + 1 >= 3.55){
            modelMatrix3.Scale(-1, 1.0, 1.0);
            direction *= -1;
        }
        
        float vertices4[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
        glEnableVertexAttribArray(program.positionAttribute);
        float texCoords4[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // butterfly set up
        program.SetModelMatrix(modelMatrix4);
        glBindTexture(GL_TEXTURE_2D, butterfly);
    
        float vertices5[] = {-0.5, -1.0, 0.5, -1.0, 0.5, 0.5, -0.5, -1.0, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices5);
        glEnableVertexAttribArray(program.positionAttribute);
        float texCoords5[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords5);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // touken set up and rocking movement
        program.SetModelMatrix(modelMatrix);
        glBindTexture(GL_TEXTURE_2D,toukenTexture);
        totalTime += 0.5 * elapsed;
        angle = elapsed * back ;
        modelMatrix.Rotate (0.5 * angle);
        //change direction of rotation if totalTime >= 0.2
        if (totalTime >= 0.2) {
            back *= -1;
            totalTime = 0;
        }
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);
        
        float vertices[] = {-0.7, -0.95, 0.7, -0.95, 0.7, 0.95, -0.7, -0.95, 0.7, 0.95, -0.7, 0.95};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
    
        // setting up a diamond star
        programUntextured.SetModelMatrix(shapes);
        programUntextured.SetViewMatrix(viewMatrixUn);
        programUntextured.SetProjectionMatrix(projectionMatrix);
        programUntextured.SetColor(0.5, 0, 0.9, 0.5f);
        glUseProgram(programUntextured.programID);
        float verticesUntextured[] = {0.85f, 0.50f, 0.80f, 0.75f, 0.75f, 0.5f, 0.85f, 0.50f, 0.75f, 0.5f, 0.80, 0.25, 0.80, 0.55, 0.80, 0.45, 1.05, 0.5, 0.80, 0.55, 0.80, 0.45, 0.55, 0.5};
        glVertexAttribPointer(programUntextured.positionAttribute, 2, GL_FLOAT, false, 0, verticesUntextured);
        glEnableVertexAttribArray(programUntextured.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glDisableVertexAttribArray(programUntextured.positionAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
