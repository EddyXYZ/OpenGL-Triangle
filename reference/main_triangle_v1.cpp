#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

int main() 
{
    // GLFW: make a window + OpenGL context
    if(!glfwInit())
    {
        return -1;
    }

    // GLFW: ask for OpenGL 3.3 core (modern OpenGL, no old fixed-function stuff)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDevelopmentProject", nullptr, nullptr);

    if(window == nullptr)
    {
        std::cout << "Error creating window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // GLFW: "draw into this window"
    glfwMakeContextCurrent(window);

    // GLEW: load modern OpenGL functions
    if(glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return -1;
    }

    // Vertex shader (GPU program, runs once per vertex):
    // takes each vertex position and outputs where it lands on screen
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;

        void main()
        {
            gl_Position = vec4(position.x, position.y, position.z, 1.0);
        }
    )";

    // OpenGL: create + compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderCStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderCStr, nullptr);
    glCompileShader(vertexShader);

    // OpenGL: check the vertex shader compiled, print errors if not
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "VERTEX_SHADER_COMPILATION_FAILED: " << infoLog << std::endl;
    }

    // Fragment shader (GPU program, runs once per pixel the triangle covers):
    // decides the color of each pixel (solid red here)
    std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

    // OpenGL: create + compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderCStr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderCStr, nullptr);
    glCompileShader(fragmentShader);

    // OpenGL: check the fragment shader compiled
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "FRAGMENT_SHADER_COMPILATION_FAILED: " << infoLog << std::endl;
    }

    // OpenGL: link vertex + fragment shaders into one shader program
    // (vertex shader output -> rasterizer -> fragment shader input)
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // OpenGL: check the program linked
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "SHADER_PROGRAM_LINKING_FAILED: " << infoLog << std::endl;
    }

    // OpenGL: shaders are copied into the program, so the originals can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // CPU: triangle vertex data (x, y, z) in normalized device coords (-1 to 1)
    std::vector<float> vertices =
    {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    // OpenGL VBO (Vertex Buffer Object): copy vertex data into GPU memory
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // OpenGL VAO (Vertex Array Object): remembers HOW to read vertices from the VBO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // OpenGL: attribute 0 ("position" in vertex shader) = 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // OpenGL: unbind so later calls don't accidentally change this VAO/VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop: runs once per frame until the window is closed
    while(!glfwWindowShouldClose(window))
    {
        // OpenGL: draw
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // OpenGL: pick shaders + vertex setup, then draw 3 vertices as 1 triangle
        // (vertex shader -> rasterize -> fragment shader -> framebuffer)
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // GLFW: show the frame, handle input
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // GLFW: destroy window + context, clean up
    glfwTerminate();


    return 0;
}