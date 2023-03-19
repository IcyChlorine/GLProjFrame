#pragma once

#include "common_t1.h"
#include <sstream>
#include <conio.h>

#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "InputManager.h"
#include "GameCamera.h"

class Application :public AbsObject
{
private:
	
protected:
	Window* window{ nullptr };
	Shader* shader{ nullptr };
	InputManager* inputManager{ nullptr };
	GameCamera* camera{ nullptr };
	Texture* texture{ nullptr };
	//stringstream consoleOutput;


	//preparing vertex data and convey them to GPU
	float vertices[32] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[6] = {  // note that we start from 0!
		0, 2, 1,
		0, 3, 2
	};
	float texCoords[6] = {
		0.0f, 0.0f,  // lower-left corner  
		1.0f, 0.0f,  // lower-right corner
		0.5f, 1.0f   // top-center corner
	};
	//generate VBO/EBO presets via VAO
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	
public:
	Application();
	~Application();
	
	void run();
	Window* getWindow() { return window; }

	/*void flushConsoleOutput() { 
		//cout << consoleOutput.str(); 
		//consoleOutput.str("");//清空. ss不能用clear.
		
	}*/
};

