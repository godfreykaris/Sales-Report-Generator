#include "background_renderer.h"
#include <iostream>

#include "../ui_dependences/stb_image/stb_image.h"


// Vertex Shader (unchanged)
const char* vertexShaderSource = R"(
    #version 130
    in vec2 position;
    in vec2 texCoord;
    out vec2 fragTexCoord;
    void main() {
        fragTexCoord = texCoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 130
    in vec2 fragTexCoord;
    out vec4 fragColor;
    uniform sampler2D backgroundTexture;
    uniform float rotationAngle;
    uniform bool useSolidColor;
    uniform vec3 solidColor;

    void main() {
        vec2 flippedCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);
        vec2 centeredCoord = flippedCoord - vec2(0.5);
        float s = sin(rotationAngle);
        float c = cos(rotationAngle);
        mat2 rotationMatrix = mat2(c, -s, s, c);
        centeredCoord = rotationMatrix * centeredCoord;
        vec2 rotatedCoord = centeredCoord + vec2(0.5);
        
        vec4 baseColor = useSolidColor ? vec4(solidColor, 1.0) : texture2D(backgroundTexture, rotatedCoord);
        baseColor.rgb *= 0.6;
        vec3 blueTint = vec3(0.1, 0.16, 0.27);
        baseColor.rgb = mix(baseColor.rgb, blueTint, 0.2);
        fragColor = baseColor;
    }
)";

// Vertex Shader for Blur (same as main)
const char* blurVertexShaderSource = R"(
    #version 130
    in vec2 position;
    in vec2 texCoord;
    out vec2 fragTexCoord;
    void main() {
        fragTexCoord = texCoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

// Fragment Shader for Blur (Horizontal and Vertical Passes)
const char* blurFragmentShaderSource = R"(
    #version 130
    in vec2 fragTexCoord;
    out vec4 fragColor;
    uniform sampler2D textureSampler;
    uniform float blurRadius;
    uniform bool horizontal;

    void main() {
        vec2 texelSize = 1.0 / vec2(textureSize(textureSampler, 0));
        vec4 result = vec4(0.0);
        float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); // Gaussian weights for 9-tap filter

        if (blurRadius <= 0.0) {
            fragColor = texture2D(textureSampler, fragTexCoord);
            return;
        }

        result += texture2D(textureSampler, fragTexCoord) * weights[0];
        for (int i = 1; i < 5; ++i) {
            vec2 offset = horizontal ? vec2(float(i) * texelSize.x * blurRadius, 0.0) : vec2(0.0, float(i) * texelSize.y * blurRadius);
            result += texture2D(textureSampler, fragTexCoord + offset) * weights[i];
            result += texture2D(textureSampler, fragTexCoord - offset) * weights[i];
        }
        fragColor = result;
    }
)";

// compileShader (unchanged)
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


// createShaderProgram
GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        return 0;
    }

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void BackgroundRenderer::compileBlurShaders() {
    blurShaderProgram = createShaderProgram(blurVertexShaderSource, blurFragmentShaderSource);
    if (blurShaderProgram == 0) {
        std::cerr << "Failed to create blur shader program" << std::endl;
    }
}


void BackgroundRenderer::compileShaders() {
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
    }
}

GLuint BackgroundRenderer::setupQuad() {
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    if (positionAttrib == -1) {
        std::cerr << "Failed to get attribute location for 'position'" << std::endl;
    }
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(positionAttrib);

    GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    if (texCoordAttrib == -1) {
        std::cerr << "Failed to get attribute location for 'texCoord'" << std::endl;
    }
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(texCoordAttrib);

    glBindVertexArray(0);
    return VAO;
}

GLuint BackgroundRenderer::loadTexture(const std::string& imagePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << imagePath << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void BackgroundRenderer::resizeFramebuffer(int width, int height) {
    // Clean up existing framebuffer and textures
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
    if (intermediateTexture) {
        glDeleteTextures(1, &intermediateTexture);
        intermediateTexture = 0;
    }
    if (finalTexture) {
        glDeleteTextures(1, &finalTexture);
        finalTexture = 0;
    }

    // Generate framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Generate intermediate texture
    glGenTextures(1, &intermediateTexture);
    glBindTexture(GL_TEXTURE_2D, intermediateTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Generate final texture
    glGenTextures(1, &finalTexture);
    glBindTexture(GL_TEXTURE_2D, finalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach intermediate texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Alias setupFramebuffer to reuse resizeFramebuffer
void BackgroundRenderer::setupFramebuffer(int width, int height) {
    resizeFramebuffer(width, height);
}

BackgroundRenderer::BackgroundRenderer(const std::string& defaultImagePath)
    : currentImageIndex(0), slideshowEnabled(false), slideshowDelta(5.0f), firstFrame(true),
      lastSwitchTime(std::chrono::high_resolution_clock::now()), blurRadius(0.0f),
      useSolidColor(false), solidColor{0.2f, 0.3f, 0.4f}, fbo(0), intermediateTexture(0), finalTexture(0) {
    compileShaders();
    compileBlurShaders();
    quadVAO = setupQuad();
    addImage(defaultImagePath);

    textureLocation = glGetUniformLocation(shaderProgram, "backgroundTexture");
    rotationAngleLocation = glGetUniformLocation(shaderProgram, "rotationAngle");
    useSolidColorLocation = glGetUniformLocation(shaderProgram, "useSolidColor");
    solidColorLocation = glGetUniformLocation(shaderProgram, "solidColor");

}

BackgroundRenderer::~BackgroundRenderer() {
    for (GLuint texture : textureIDs) {
        glDeleteTextures(1, &texture);
    }
    glDeleteTextures(1, &intermediateTexture);
    glDeleteTextures(1, &finalTexture);
    glDeleteFramebuffers(1, &fbo);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(blurShaderProgram);
    glDeleteVertexArrays(1, &quadVAO);
}


void BackgroundRenderer::addImage(const std::string& imagePath) {
    // Check for duplicate image path
    auto it = std::find(imagePaths.begin(), imagePaths.end(), imagePath);
    if (it == imagePaths.end()) {
        GLuint texture = loadTexture(imagePath);
        if (texture != 0) {
            textureIDs.push_back(texture);
            imagePaths.push_back(imagePath);
            if (textureIDs.size() == 1) {
                currentImageIndex = 0;
            }
        }
    }
}

void BackgroundRenderer::removeImage(size_t index) {
    if (index < textureIDs.size() && textureIDs.size() > 1) {
        glDeleteTextures(1, &textureIDs[index]);
        textureIDs.erase(textureIDs.begin() + index);
        imagePaths.erase(imagePaths.begin() + index);
        if (currentImageIndex >= textureIDs.size()) {
            currentImageIndex = textureIDs.size() - 1;
        }
    }
}

void BackgroundRenderer::setCurrentImage(size_t index) {
    if (index < textureIDs.size()) {
        currentImageIndex = index;
    }
}

void BackgroundRenderer::enableSlideshow(bool enable, float delta) {
    slideshowEnabled = enable;
    slideshowDelta = delta > 0.0f ? delta : 5.0f;
    lastSwitchTime = std::chrono::high_resolution_clock::now(); // Reset timer
}

void BackgroundRenderer::update() {
    if (!slideshowEnabled || textureIDs.size() <= 1) {
        return;
    }

    // Get current system time
    auto now = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float>(now - lastSwitchTime).count();

    // Check if elapsed time exceeds slideshowDelta
    if (elapsed >= slideshowDelta) {
        currentImageIndex = (currentImageIndex + 1) % textureIDs.size();
        lastSwitchTime = now; // Reset timer
    }
}

void BackgroundRenderer::render() {
    if (textureIDs.empty() && !useSolidColor) return;

    if (!firstFrame) {
        update();
    }
    firstFrame = false;

    // Step 1: Render base image/solid color to intermediate texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    if (!useSolidColor && !textureIDs.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIDs[currentImageIndex]);
        glUniform1i(textureLocation, 0);
    }
    glUniform1i(useSolidColorLocation, useSolidColor ? 1 : 0);
    glUniform3fv(solidColorLocation, 1, solidColor);
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    if (!useSolidColor) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Step 2: Horizontal blur pass to final texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blurShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediateTexture);
    glUniform1i(glGetUniformLocation(blurShaderProgram, "textureSampler"), 0);
    glUniform1f(glGetUniformLocation(blurShaderProgram, "blurRadius"), blurRadius * 10.0f); // Scale for effect
    glUniform1i(glGetUniformLocation(blurShaderProgram, "horizontal"), 1);
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Step 3: Vertical blur pass to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blurShaderProgram);
    glBindTexture(GL_TEXTURE_2D, finalTexture);
    glUniform1i(glGetUniformLocation(blurShaderProgram, "textureSampler"), 0);
    glUniform1f(glGetUniformLocation(blurShaderProgram, "blurRadius"), blurRadius * 10.0f);
    glUniform1i(glGetUniformLocation(blurShaderProgram, "horizontal"), 0);
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
