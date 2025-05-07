#pragma once

#include "../ui_dependences/glew/include/glew.h"
#include "../ui_dependences/glfw/include/glfw/glfw3.h"
#include <string>
#include <vector>
#include <chrono>

extern bool login_window;

class BackgroundRenderer {
public:
    BackgroundRenderer(const std::string& defaultImagePath);
    ~BackgroundRenderer();

    void render();
    void addImage(const std::string& imagePath);
    void removeImage(size_t index);
    void setCurrentImage(size_t index);
    void enableSlideshow(bool enable, float delta = 5.0f);
    void update();
    size_t getImageCount() const { return imagePaths.size(); }
    const std::vector<std::string>& getImagePaths() const { return imagePaths; }
    size_t getCurrentImageIndex() const { return currentImageIndex; }

    void setBlurRadius(float radius) { blurRadius = radius; }
    float getBlurRadius() const { return blurRadius; }
    void setSolidColorMode(bool enable) { useSolidColor = enable; }
    bool isSolidColorMode() const { return useSolidColor; }
    void setSolidColor(float r, float g, float b) { solidColor[0] = r; solidColor[1] = g; solidColor[2] = b; }
    const float* getSolidColor() const { return solidColor; }
    
    void resizeFramebuffer(int width, int height);


private:
    GLuint shaderProgram;
    GLuint blurShaderProgram; // New shader for blur passes
    GLuint quadVAO;
    std::vector<GLuint> textureIDs;
    std::vector<std::string> imagePaths;
    size_t currentImageIndex;
    GLint textureLocation;
    GLint rotationAngleLocation;
    GLint blurRadiusLocation;
    GLint useSolidColorLocation;
    GLint solidColorLocation;
    bool slideshowEnabled;
    float slideshowDelta;
    bool firstFrame;
    std::chrono::high_resolution_clock::time_point lastSwitchTime;
    float blurRadius;
    bool useSolidColor;
    float solidColor[3];

    // Framebuffer for blur
    GLuint fbo; // Framebuffer object
    GLuint intermediateTexture; // Texture for horizontal blur pass
    GLuint finalTexture; // Texture for final output

    void compileShaders();
    void compileBlurShaders();
    GLuint setupQuad();
    GLuint loadTexture(const std::string& imagePath);
    void setupFramebuffer(int width, int height);
};