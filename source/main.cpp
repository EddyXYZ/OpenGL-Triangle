// ----------------------------------------------------------------------
//      Stage 0: setup GLFW and glew (glew is always included first)
// ----------------------------------------------------------------------

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

float vertices[] = {
    // positions        // colors
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // bottom left
    0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
    layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
    
    out vec3 ourColor; // output a color to the fragment shader

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor; // set ourColor to the input color we got from the vertex data
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;  
    in vec3 ourColor;
    
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)";

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
    /*
        Functions used: glfwInit, glfwWindowHint, glfwCreateWindow,
                        glfwTerminate, glfwMakeContextCurrent
    */
    // ----------------------------------------------------------------------

    // Initialize the GLFW library, set the GLFW major and minor versions, and use OpenGL core
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and check if the window failed to create
    GLFWwindow* window = glfwCreateWindow(800, 600, "GameDevelopmentProject", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make this OpenGL state (also known as "context") the current one
    glfwMakeContextCurrent(window);

    // ----------------------------------------------------------------------
    //      Stage 2: load/initialize the OpenGL functions with GLEW
    /*
        Functions used: glewInit, glfwTerminate, glGetString
        glfwSetFramebufferSizeCallback, framebufferSizeCallback
    */
    // ----------------------------------------------------------------------
    // All GL functions should be called once there is a current context and GLEW is initialized

    // Check if the initialization for GLEW failed
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize the GLEW library" << std::endl;
        glfwTerminate();
        return -1;
    }

    // GL_VERSION and GL_RENDERER are available if there is a current context
    std::cout << "Version: " << glGetString(GL_VERSION) << " | " << "Renderer: " <<glGetString(GL_RENDERER) << std::endl;

    // Adjust the viewport if the user resizes it
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // ----------------------------------------------------------------------
    //               Stage 4: put the vertex data in a VBO
    /*
        Functions used: glGenBuffers, glBindBuffer, glBufferData
    */
    // ----------------------------------------------------------------------

    /*
    Example:
    --------
    GLuint vbo;                   vbo = ?            GPU: (nothing)
    glGenBuffers(1, &vbo);        vbo = 1            GPU: ID 1 reserved
    glBindBuffer(..., vbo);       GL_ARRAY_BUFFER → buffer 1
    glBufferData(...);            GPU: buffer 1 = [# bytes of your vertex data]
    */

    // Check the amount of vertices
    std::cout << "Vertices: " << sizeof(vertices) / sizeof(float) / 3 << std::endl; 

    // Create a VBO
    GLuint vbo; // - This creates an empty variable that will hold the VBO’s ID
    glGenBuffers(1, &vbo); // - Generate 1 buffer ID and write it into vbo

    // Binding the ID for the first time creates the buffer object (still empty)
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // - Bind vbo to GL_ARRAY_BUFFER so calls on GL_ARRAY_BUFFER act on vbo

    // Allocate GPU memory into the buffer ID that is currently bound to GL_ARRAY_BUFFER and copy the vertices data into it
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ----------------------------------------------------------------------
    //          Stage 5: describe the VBO’s using a VAO
    /*
        Functions used: glGenVertexArrays, glBindVertexArray, glBindBuffer,
                        glVertexAttribPointer, glEnableVertexAttribArray,
                        glGenBuffers
    */
    // ----------------------------------------------------------------------
    // VBO = the data. VAO = the instructions for reading it
    // the VAO links to the VBO because the VAO holds the instructions for reading it

    // Create the VAO
    GLuint vao; // - This creates an empty variable that will hold the VAO’s ID
    glGenVertexArrays(1, &vao); // - Generate 1 buffer ID and write it into vao

    // Binding the ID for the first time creates the VAO (still empty)
    // While bound, attribute calls and the EBO write their settings into the VAO
    // These attribute calls are: glVertexAttribPointer and glEnableVertexAttribArray
    glBindVertexArray(vao); 

    // Bind the VBO (again) so glVertexAttribPointer links this VAO to it (makes the dependency explicit)
    // When glVertexAttribPointer runs, it saves into the VAO the ID of the buffer GL_ARRAY_BUFFER refers to
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 

    // This is where the VAO describes how to read the VBO (size, type, stride, offset)
    // --------------------------------------------------------------------------------

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0); // - Turn on attribute 0 (off by default) so the vertex shader receives aPos

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1); // - Turn on attribute 1 (off by default) so the vertex shader receives aColor

    /*
    Example:
    --------
                            the picture's row   its numbers         where it ends up
    location 0  ──►   POSITION (red)      offset 0,  stride 24  ──►  aPos
    location 1  ──►   COLOR (green)       offset 12, stride 24  ──►  aColor
    */

    // Deselect the VAO because its settings were written by the calls above
    // Unbinding stops later attributes calls + EBO binds from changing the VAO
    glBindVertexArray(0); 

    // ----------------------------------------------------------------------
    //                     Stage 6: the vertex shader
    /*
        Functions used: glCreateShader, glShaderSource, glCompileShader,
                        glGetShaderiv, glGetShaderInfoLog
    */
    // ----------------------------------------------------------------------
    // a small program, which runs once per vertex on the GPU, that outputs the vertex’s position in gl_Position
    // gl_Position is used as the output of the vertex shader

    // Create a shader object 
    GLuint vertexShader; // - This creates an empty variable that will hold the vertex shader's ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // - Creates an empty shader object with the given shader type

    // Store the vertex shader (using its GLSL text) in the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    // Driver compiles the text into GPU code
    glCompileShader(vertexShader);

    // Always check if the shader complied
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // - Get the shader's compile status and store it in success

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader failed to compile: " << infoLog << std::endl;
    }

    // ----------------------------------------------------------------------
    //                     Stage 6: the fragment shader
    /*
        Functions used: glCreateShader, glShaderSource, glCompileShader,
                        glGetShaderiv, glGetShaderInfoLog
    */
    // ----------------------------------------------------------------------
    // The fragment shader runs once per fragment and calculates the color output of pixels
    

    // Create a shader object
    GLuint fragmentShader; // - This creates an empty variable that will hold the fragment shader's ID
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // - Creates an empty shader object with the given shader type
    
    // Store the fragment shader (using its GLSL text) into the shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    // Driver compiles the text into GPU code
    glCompileShader(fragmentShader);

    // Always check if the shader complied
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // - Get the shader's compile status and store it in success
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader failed to compile: " << infoLog << std::endl;
    }

    // ----------------------------------------------------------------------
    //                     Stage 7: the shader program
    /*
        Functions used: glCreateProgram, glAttachShader, glLinkProgram,
                        glGetProgramiv, glGetProgramInfoLog, glDeleteShader
    */
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
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // - Get the program's compile status and store it in success
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
    /*
        Functions used: glPolygonMode, glfwWindowShouldClose, glfwPollEvents,
                        processInput, glClearColor, glClear, glUseProgram,
                        glBindVertexArray, glDrawElements, glfwSwapBuffers, 
                        lfwTerminate
    */
    // ----------------------------------------------------------------------
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window))
    {
        // ----- Poll ----- //

        glfwPollEvents();

        // ----- Update ----- //

        processInput(window);

        // ----- Render ----- //

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // - Set the "clear color"
        glClear(GL_COLOR_BUFFER_BIT); // - Back buffer is filled with the "clear color"

        // Activate the shader program object
        // Every render call after glUseProgram() will now use this program object
        glUseProgram(shaderProgram);

        // Bind the VAO so glDrawArrays knows which vertex data to use and how to read it
        glBindVertexArray(vao);

        // Draw 3 vertices from the bound VAO as triangles, starting at vertex 0 (this starts the pipeline)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Show the finished frame: swap the back buffer (just drawn into) with the front buffer (on screen)
        glfwSwapBuffers(window);
    }

    glfwTerminate();
   
    return 0;
}