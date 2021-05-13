#include <iostream>
#include <cmath>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shaderClass.h>
#include <vao.h>
#include <vbo.h>
#include <ebo.h>
#include <texture.h>
#include <camera.h>

// colors of rubik cube
// blue =   0.0f, 0.0f, 1.0f,
// white =  1.0f, 1.0f, 1.0f,
// yellow = 1.0f, 1.0f, 0.0f,
// green =  0.0f, 0.5f, 0.0f,
// orange = 1.0f, 0.5f, 0.0f,
// red =    1.0f, 0.0f, 0.0f,
GLfloat Vertices[] =
{ //     COORDINATES        //  COLORS
	-0.1f, -0.1f,  0.1f,    1.0f, 0.0f, 0.0f,       //0   
	-0.1f, -0.1f, -0.1f,    1.0f, 0.0f, 0.0f,       //1
	 0.1f, -0.1f, -0.1f,    1.0f, 1.0f, 0.0f,       //2
	 0.1f, -0.1f,  0.1f,    1.0f, 1.0f, 0.0f,       //3
	-0.1f,  0.1f,  0.1f,    0.0f, 0.0f, 1.0f,       //4
	-0.1f,  0.1f, -0.1f,    0.0f, 0.0f, 1.0f,       //5
	 0.1f,  0.1f, -0.1f,    1.0f, 0.0f, 1.0f,       //6
	 0.1f,  0.1f,  0.1f,    1.0f, 0.0f, 1.0f,       //7
};

GLuint Indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

int width = 800, height = 800;

int main()
{
	// Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	GLFWwindow* window = glfwCreateWindow(width, height, "Window", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);


	// Shader for cube
	Shader Shader("shader.vert", "shader.frag");
	// Generates Vertex Array Object and binds it
	VAO VAO;
	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(Vertices, sizeof(Vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO(Indices, sizeof(Indices));
	// Links VBO attributes such as coordinates and colors to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();


	glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::translate(Model, Pos);

	Shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(Shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(Model));

	// camera
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));


	// positions for the cube
	std::vector<glm::vec3> positions(27);
	int n = 2, pos = 0;
	for (int i = -1; i < n; ++i) {
		for (int j = -1; j < n; ++j) {
			for (int k = -1; k < n; ++k) {
				float x = 0.1f;
				float z = 0.1f;
				float y = 0.1f;
				
				positions[(pos++)] = glm::vec3(i * x, j * y, k * z);
			}
		}
	}


	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// background color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		// Manage inputs for camera
		camera.Inputs(window);
		
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		Shader.Activate();
		camera.Matrix(Shader, "camMatrix");
		VAO.Bind();

		//draw all the cubes
		for (int i = 0; i < positions.size(); ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, positions[i]);

			glUniformMatrix4fv(glGetUniformLocation(Shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, sizeof(Indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		}
		
		// Swap the window buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	// Deletes
	VAO.Delete();
	VBO.Delete();
	EBO.Delete();
	Shader.Delete();
    
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
