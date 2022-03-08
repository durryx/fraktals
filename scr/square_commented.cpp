#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// SHADER

// vertex shader source
// declaration of input vertex attributes, since it is a 3D a 
// three dimensional vector is used (vec3)
// the exact location of the variable is indicated with layout (location=0)
const char *vertexShaderSource = "#version 330 core\n"	// opengl version
"layout (location = 0) in vec3 aPos;\n"
"void main(){\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n\0";

// fragment shader source
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main(){\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


// WINDOW

// checks whether escape is pressed, window should close
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// sets viewport to window dimensions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// viewport match window dimensions
	glViewport(0, 0, width, height);
}


// window's dimensions for GLFW
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main(){
	// initialize GLFW
	glfwInit();
	// indicate major opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// indicate minor opengl version, 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// using core profile opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Title", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		return -1;
	}
	// set current window as context
	glfwMakeContextCurrent(window);
	// set viewport to window dimensions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// load glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -1;
	}
	
	// compiling shader program
	
	// vertex shader
	
	// assign ID of shader program to be compiled at run time
	unsigned int vertexshader = glCreateShader(GL_VERTEX_SHADER);
	// compiles the given string (the second arguments indicates how many 
	// strings were given) into vertexshader
	glShaderSource(vertexshader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexshader);
	
	//check if compilation succeed
	int success;
	char log[512];
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexshader, 512, NULL, log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << log << std::endl;
	}
	
	// fragment shader (color out the pixels)
	// almost same operations
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	//check for compilation errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    	if (!success)
    	{
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << log << std::endl;
    	}
	
	// link shaders
	// get ID of final linked shader
	unsigned int shaderProgram = glCreateProgram();
	// link both vertex and fragment shader
	glAttachShader(shaderProgram, vertexshader);
	glAttachShader(shaderProgram, fragmentShader);
	//
	glLinkProgram(shaderProgram);

	//check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    	if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << log << std::endl;
	}

	// delete both shaders after linking
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentShader);

	// first bind VAO (Vertex array object), then VAO (vertex buffer),
	// finally the EBO ()
	
	// declaring vertex data
	float vertices[] = {
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};	
	// EBO stuff (how to render the given vertices) indices of the given vertices
	unsigned int indices[] = {
		0, 1, 3,	//first triangle
		1, 2, 3		//second triangle
	};

	unsigned int VAO, VBO, EBO;
	
	// generating VAO, VBO, EBO ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind VAO 
	glBindVertexArray(VAO);

	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// tell opengl how to interpret vertex data (per vertex attribute)
	// the 1° parameter specifies the location of the vertex attribute
	// --> in the vertex shader layout (location=0) was specified
	// 2° parameter indicates the size of the vertex attribute 
	// --> vec3 in the shader is composed of the 3 values
	// // 3° argument specifies the type of data
	// --> in this case float
	// the 5° argument is stride, the (memory )space between each different
	// vertex attributes
	// --> a vertex in this case is a 3 dimensional float vector
	// 6° is the offset, where in memory the position data starts
	// --> in this case 0 (a cast)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// do not unbind the EBO while VAO is active
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind VAO
	glBindVertexArray(0);
	
	// wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// sets window should close to true when key is pressed
		processInput(window);
		
		//clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// tell opnegl to use this shader
		glUseProgram(shaderProgram);
		// rebind VAO
		glBindVertexArray(VAO);
		// finally draw elements
		// glDrawElements takes indices from the EBO bound to
		// GL_ELEMENT_ARRAY_BUFFER (in this case 6 indices)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		// glfw swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// deallocate everything
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	
	// clear all previously GLFW resources
	glfwTerminate();
	return 0;
}

