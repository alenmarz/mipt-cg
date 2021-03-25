#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 512, "Pyramid", NULL, NULL);
    if ( window == NULL ) {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);f
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL programs from the shaders
    GLuint programID = LoadShaders( "VertexShader.vertexshader", "FragmentShader.fragmentshader" );
    
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation( programID, "MVP" );
    
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective( 45.05f, 2.0f / 1.0f, 0.1f, 100.0f );
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4( 1.0f );

    // 2 triangles
    static const GLfloat g_vertex_buffer_data[] = {
        // #1
        -0.5f, -0.5f,  0.5f,
         0.0f,  0.5f,  0.0f,
         0.5f, -0.5f,  0.5f,
        // #2
         0.5f, -0.5f,  0.5f,
         0.0f,  0.5f,  0.0f,
         0.5f, -0.5f, -0.5f,
        // #3
         0.5f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.0f,
        -0.5f, -0.5f, -0.5f,
        // #4
        -0.5f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.0f,
        -0.5f, -0.5f,  0.5f,
        // #5
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        // #6
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f
    };
    
    static const GLfloat g_color_buffer_data[] = {
        // #1
        0.266f,  0.133f,  0.533f,
        0.709f,  0.827f,  0.239f,
        0.423f,  0.635f,  0.917f,
        // #2
        0.423f,  0.635f,  0.917f,
        0.709f,  0.827f,  0.239f,
        0.921f,  0.490f,  0.356f,
        // #3
        0.921f,  0.490f,  0.356f,
        0.709f,  0.827f,  0.239f,
        0.996f,  0.823f,  0.247f,
        // #4
        0.996f,  0.823f,  0.247f,
        0.709f,  0.827f,  0.239f,
        0.266f,  0.133f,  0.533f,
        // #5
        0.266f,  0.133f,  0.533f,
        0.996f,  0.823f,  0.247f,
        0.423f,  0.635f,  0.917f,
        // #6
        0.423f,  0.635f,  0.917f,
        0.996f,  0.823f,  0.247f,
        0.921f,  0.490f,  0.356f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    GLuint colorbuffer;
    glGenBuffers( 1, &colorbuffer );
    glBindBuffer( GL_ARRAY_BUFFER, colorbuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof( g_color_buffer_data ), g_color_buffer_data, GL_STATIC_DRAW );

    double step = 0.02;
    double camera_angle = 0;
    
    do {
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Camera matrix
        glm::mat4 View = glm::lookAt(
            glm::vec3( cos( camera_angle ), sin( camera_angle ), sin( camera_angle ) ), // Camera is rotating, in World Space
            glm::vec3( 0, 0, 0 ),
            glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
        );
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
        
        // Use our shader
        glUseProgram( programID );

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv( MatrixID, 1, GL_FALSE, &MVP[0][0] );

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*) 0           // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray( 1 );
        glBindBuffer( GL_ARRAY_BUFFER, colorbuffer );
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*) 0                         // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays( GL_TRIANGLES, 0, 6 * 3 ); // 6*3 indices starting at 0 -> 6 triangles

        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        
        // Swap buffers
        glfwSwapBuffers( window );
        glfwPollEvents();

        camera_angle += step;
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram( programID );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

