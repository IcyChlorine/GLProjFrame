#pragma once
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "common_t1.h"
#include <sstream>
#include <map>

#include "glpp/Window.h"
#include "glpp/Shader.h"
#include "glpp/Texture.h"
#include "glpp/InputManager.h"
#include "glpp/GameCamera.h"

class GraphicApplicationBase: 
	public AbsObject, public Singleton
{
private:
	
protected:
	Window* window{ nullptr };
	InputManager* inputManager{ nullptr };
	
public:
	static GraphicApplicationBase* instance;//implemtation of "Singleton"
	GraphicApplicationBase();
	~GraphicApplicationBase();
	
	virtual void run() {}
	Window* getWindow() { return window; }
};

class Application :public GraphicApplicationBase
{
private:
	
protected:
	//base class has hadled these for us
	//Window* window{ nullptr };
	//InputManager* inputManager{ nullptr };

	Shader* shader{ nullptr };	
	GameCamera* camera{ nullptr };
	bool dragging{false};//相机控制相关
	float prev_cursor_x{0.},prev_cursor_y{0.};
	map<int, bool> key_pressed;

	bool require_FPS_info{false};

	Texture* texture{ nullptr };

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

#endif //_APPLICATION_H