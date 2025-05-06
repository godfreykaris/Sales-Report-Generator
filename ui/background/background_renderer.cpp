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

// Updated Fragment Shader (with vertical flip, rotation, and Gaussian blur)
const char* fragmentShaderSource = R"(
    #version 130
    in vec2 fragTexCoord;
    out vec4 fragColor;
    uniform sampler2D backgroundTexture;
    uniform float rotationAngle; // Angle in radians for rotating the texture
    uniform float blurRadius; // Control the blur intensity

    // Gaussian blur kernel (5x5 approximation)
    vec4 applyGaussianBlur(vec2 uv) {
        vec4 color = vec4(0.0);
        float kernel[25] = float[](
            1.0, 4.0, 7.0, 4.0, 1.0,
            4.0, 16.0, 26.0, 16.0, 4.0,
            7.0, 26.0, 41.0, 26.0, 7.0,
            4.0, 16.0, 26.0, 16.0, 4.0,
            1.0, 4.0, 7.0, 4.0, 1.0
        );
        float kernelSum = 273.0; // Sum of all kernel values

        // Sample a 5x5 grid around the current pixel
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                vec2 offset = vec2(float(i), float(j)) * blurRadius;
                color += texture2D(backgroundTexture, uv + offset) * kernel[(i + 2) * 5 + (j + 2)];
            }
        }
        return color / kernelSum;
    }

    void main() {
        // Flip the texture vertically to correct stb_image orientation
        vec2 flippedCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);

        // Center the texture coordinates for rotation
        vec2 centeredCoord = flippedCoord - vec2(0.5);

        // Apply rotation
        float s = sin(rotationAngle);
        float c = cos(rotationAngle);
        mat2 rotationMatrix = mat2(c, -s, s, c);
        centeredCoord = rotationMatrix * centeredCoord;

        // Shift back to [0,1] range
        vec2 rotatedCoord = centeredCoord + vec2(0.5);

        // Apply Gaussian blur
        vec4 blurredColor = applyGaussianBlur(rotatedCoord);

        // Darken and tint for better integration with the dashboard
        blurredColor.rgb *= 0.6; // Reduce brightness by 40%
        vec3 blueTint = vec3(0.1, 0.16, 0.27); // Approx #1A2A44
        blurredColor.rgb = mix(blurredColor.rgb, blueTint, 0.2); // Blend 20% blue tint

        fragColor = blurredColor;
    }
)";

// Function to compile shaders with error checking (unchanged)
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

// Function to create shader program with error checking (unchanged)
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

BackgroundRenderer::BackgroundRenderer(const std::string& imagePath) {
    // Compile shaders and set up the quad
    compileShaders();
    quadVAO = setupQuad();

    // Load the texture
    textureID = loadTexture(imagePath);

    // Get uniform locations
    textureLocation = glGetUniformLocation(shaderProgram, "backgroundTexture");
    if (textureLocation == -1) {
        std::cerr << "Failed to get uniform location for 'backgroundTexture'" << std::endl;
    }

    rotationAngleLocation = glGetUniformLocation(shaderProgram, "rotationAngle");
    if (rotationAngleLocation == -1) {
        std::cerr << "Failed to get uniform location for 'rotationAngle'" << std::endl;
    }

    blurRadiusLocation = glGetUniformLocation(shaderProgram, "blurRadius");
    if (blurRadiusLocation == -1) {
        std::cerr << "Failed to get uniform location for 'blurRadius'" << std::endl;
    }
}

BackgroundRenderer::~BackgroundRenderer() {
    // Cleanup
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteTextures(1, &textureID);
}

void BackgroundRenderer::compileShaders() {
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
    }
}

GLuint BackgroundRenderer::setupQuad() {
    float vertices[] = {
        // Positions     // Texture Coordinates
        -1.0f, -1.0f,    0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f,    1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f,    1.0f, 1.0f, // Top-right
        -1.0f,  1.0f,    0.0f, 1.0f  // Top-left
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

    // Bind the 'position' attribute (location 0)
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    if (positionAttrib == -1) {
        std::cerr << "Failed to get attribute location for 'position'" << std::endl;
    }
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(positionAttrib);

    // Bind the 'texCoord' attribute (location 1)
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

    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image using stb_image
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

void BackgroundRenderer::render() {
    // Render the texture background
    glUseProgram(shaderProgram);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureLocation, 0);

    float blurRadius = 0.000; // Subtle blur effect
    glUniform1f(blurRadiusLocation, blurRadius);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}