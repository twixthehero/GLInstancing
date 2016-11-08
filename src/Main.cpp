#include "Main.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "Utils.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Shader.h"

void init()
{
	glfwSetKeyCallback(window, key);

	glEnable(GL_BLEND);	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);

	glClearColor(0.2f, 0.0f, 0.0f, 0.0f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float data[] =
	{
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f
	};
	int indices[] =
	{
		//bottom
		0, 1, 2,
		1, 3, 2,
		//top
		4, 6, 5,
		6, 7, 5,
		//back
		5, 1, 0,
		5, 0, 4,
		//front
		6, 2, 7,
		7, 2, 3,
		//left
		4, 0, 6,
		6, 0, 2,
		//right
		7, 3, 5,
		5, 3, 1
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	defaultShader = new Shader(0, "default");
	instanceShader = new Shader(1, "instance");

	initSizing();

	proj = glm::perspective(glm::radians(60.0f), 1280.0f / 720, 0.05f, 500.0f);

	hsv = glm::vec3(hue / 1.0f, 1.0f, 1.0f);
	scale = glm::scale(glm::mat4(), glm::vec3(1));
}

void initSizing()
{
	if (model != nullptr)
		delete[] model;

	if (useInstanceRendering)
	{
		totalCubes = (numCubes * 2 + 1) * (numCubes * 2 + 1) * (numCubes * 2 + 1);
		model = new glm::mat4[totalCubes];

		currentShader = instanceShader;
		currentShader->Bind();
	}
	else
	{
		totalCubes = 1;
		model = new glm::mat4[totalCubes];

		currentShader = defaultShader;
		currentShader->Bind();
	}
}

void update()
{
	hue += shiftSpeed * dt;

	if (hue >= 1)
		hue = 0;

	hsv.x = hue * 255.0f;
	rgb = Utils::HSVtoRGB(hsv);

	rot += rotateSpeed * dt;
	if (rot >= 360)
		rot = 0;

	camRot += camRotateSpeed * dt;
	if (camRot >= 360)
		camRot = 0;

	view = glm::lookAt(glm::vec3(glm::cos(glm::radians(camRot)) * 5, 0, glm::sin(glm::radians(camRot)) * 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	currentShader->SetUniformMatrix4f("proj", proj);

	if (useInstanceRendering)
	{
		int index = 0;
		std::string name;
		float xRot = rot + 18;
		rotation = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(xRot), glm::radians(rot), 0)));

		for (int x = -numCubes; x <= numCubes; x++)
			for (int y = -numCubes; y <= numCubes; y++)
				for (int z = -numCubes; z <= numCubes; z++, index++)
				{
					translation = glm::translate(glm::mat4(), glm::vec3(x * 5, y * 5, z * 5));
					model[index] = translation * rotation * scale;

					name = "model[" + std::to_string(index) + "]";
					currentShader->SetUniformMatrixInstance4f("model", index, model[index]);
				}
	}
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentShader->Bind();

	glBindVertexArray(vao);

	currentShader->SetUniform3f("color", rgb);
	currentShader->SetUniformMatrix4f("view", view);

	if (useInstanceRendering)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, totalCubes);
	}
	else
	{
		float xRot = rot + 18;
		rotation = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(xRot), glm::radians(rot), 0)));

		for (int x = -numCubes; x <= numCubes; x++)
			for (int y = -numCubes; y <= numCubes; y++)
				for (int z = -numCubes; z <= numCubes; z++)
				{
					translation = glm::translate(glm::mat4(), glm::vec3(x * 5, y * 5, z * 5));
					model[0] = translation * rotation * scale;
					currentShader->SetUniformMatrix4f("model", model[0]);

					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				}
	}
}

void cleanup()
{
	delete defaultShader;
	delete instanceShader;
	delete model;
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_V:
		if (action == GLFW_RELEASE)
		{
			useInstanceRendering = !useInstanceRendering;

			if (useInstanceRendering)
			{
				currentShader = instanceShader;
				glfwSetWindowTitle(window, "OpenGL - Instancing");
			}
			else
			{
				currentShader = defaultShader;
				glfwSetWindowTitle(window, "OpenGL");
			}

			initSizing();
		}
	}
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to init GLFW" << std::endl;
		return -1;
	}

	window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);

	if (!window)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init glew" << std::endl;
		glfwTerminate();
		return -1;
	}

	init();

	double time;

	while (!glfwWindowShouldClose(window))
	{
		time = glfwGetTime();
		dt = time - lastTime;
		lastTime = time;

		update();
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanup();

	glfwTerminate();

	return 0;
}