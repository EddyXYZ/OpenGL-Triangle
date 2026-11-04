// ----------------------------------------------------------------------
//      Stage 0: setup GLFW and glew (glew is always included first)
// ----------------------------------------------------------------------

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    // ----------------------------------------------------------------------
    //       Stage 1: create a window and a OpenGL context with GLFW
    // ----------------------------------------------------------------------
    
    if(!glfwInit())
    {
        std::cout << "Failed to initialize GLFW library" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GameDevelopmentProject", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ----------------------------------------------------------------------
    //      Stage 2: load/initialize the OpenGL functions with GLEW
    // ----------------------------------------------------------------------
    
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW library" << std::endl;
        glfwTerminate();
        return -1;
    }
    // std::cout << glGetString(GL_VERSION) << " " << glGetString(GL_RENDERER) << std::endl;

    // Adjust the viewport if the user resizes it
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // ----------------------------------------------------------------------
    //                     Stage 4: put the vertex data in a VBO 
    // ----------------------------------------------------------------------
    
    float vertices[] = {
        0.f, 0.5f, 0.0f, 
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    GLuint vbo; // This creates an empty variable that will hold the buffer’s ID
    glGenBuffers(1, &vbo); // Generate 1 buffer name and write it into vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Make buffer vbo the current one for the GL_ARRAY_BUFFER target
    
    // Allocate GPU memory for whatever buffer is bound to GL_ARRAY_BUFFER, and copy my data into it
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ----------------------------------------------------------------------
    //                             Example
    // ----------------------------------------------------------------------
    // GLuint vbo;                   vbo = ?            GPU: (nothing)
    // glGenBuffers(1, &vbo);        vbo = 1            GPU: ID 1 reserved
    // glBindBuffer(..., vbo);       GL_ARRAY_BUFFER → buffer 1
    // glBufferData(...);            GPU: buffer 1 = [36 bytes of your vertex data]
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    //                     Stage 5: describe the VBO’s data
    // ----------------------------------------------------------------------
    // VBO = the data. VAO = the instructions for reading it.
    // the VAO links to the VBO because the VAO holds the instructions for reading it

    GLuint vao;
    glGenVertexArrays(1, &vao); // Reserve a VAO ID

    // Select this VAO: the attribute calls below now write their settings into it
    // These attribute calls are: glVertexAttribPointer and glEnableVertexAttribArray
    glBindVertexArray(vao); 
    
    // Bind the VBO so glVertexAttribPointer links this VAO to it (makes the dependency explicit)
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 

    // This is where the VAO describes how to read the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glEnableVertexAttribArray(0); // Turn attribute 0 on (attributes are off by default)

    // Deselect the VAO: its settings are saved and later attribute calls can't change it
    glBindVertexArray(0); 

    // Unbind the VBO (the VAO already saved its link to it)
    // Prevent later code from accidently writing into the current VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ----------------------------------------------------------------------
    //                     Stage 6: the vertex shader
    // ----------------------------------------------------------------------
    // a small program, which runs once per vertex on the GPU, that outputs the vertex’s position in gl_Position
    // gl_Position is used as the output of the vertex shader

    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";

    // Create a shader object 
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Store the vertex shader (using its source) in the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Always check if the shader complied
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader failed to compile: " << infoLog << std::endl;
    }

    // ----------------------------------------------------------------------
    //                     Stage 6: the fragment shader
    // ----------------------------------------------------------------------
    // The fragment shader runs once per fragment and calculates the color output of pixels
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";

    // Create a shader object
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Store the fragment shader (using its source) into the shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Always check if the shader complied
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader failed to compile: " << infoLog << std::endl;
    }

    // ----------------------------------------------------------------------
    //                     Stage 7: the shader program
    // ----------------------------------------------------------------------
    // The shader program is the final linked version of multiple shaders

    // Create the shader program object
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    // Attach the compiled shaders to the shader program object
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link the compiled shaders within the shader program object
    glLinkProgram(shaderProgram);

    // Always check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program failed to link: " << infoLog << std::endl;
    }

    // Delete the shader objects once they're linked to the shader program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ----------------------------------------------------------------------
    //                     Stage 3: the render loop
    // ----------------------------------------------------------------------
    
    while(!glfwWindowShouldClose(window))
    {
        // Poll
        glfwPollEvents();

        // Update
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set the "clear color"
        glClear(GL_COLOR_BUFFER_BIT); // Back buffer is filled with the "clear color"
        
        // Activate the shader program object
        // Every render call after glUseProgram() will now use this program object
        glUseProgram(shaderProgram);

        // Bind the VAO so glDrawArrays knows which vertex data to use and how to read it
        glBindVertexArray(vao);

        // Render
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}