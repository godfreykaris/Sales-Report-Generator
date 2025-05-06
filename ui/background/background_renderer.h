#include "../ui_dependences/glew/include/glew.h"
#include "../ui_dependences/glfw/include/glfw/glfw3.h"

#include <string>

class BackgroundRenderer {
public:
    BackgroundRenderer(const std::string& imagePath);
    ~BackgroundRenderer();

    void render();

private:
    GLuint shaderProgram;
    GLuint quadVAO;
    GLuint textureID;
    GLint textureLocation;
    GLint rotationAngleLocation; // Uniform location for rotation angle
    GLint blurRadiusLocation;   // Uniform location for blur radius

    void compileShaders();
    GLuint setupQuad();
    GLuint loadTexture(const std::string& imagePath);
};