#pragma once
#ifndef _APP_H
#define _APP_H

#include "common_t1.h"
#include <sstream>
#include <conio.h>

#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderable.h"
#include "Text.h"
#include "InputManager.h"
#include "GameCamera.h"

class Application :public AbsObject
{
private:
	bool show_hud{ false };
	bool line_mode{ false };
protected:
	Window* window{ nullptr };
	InputManager* inputManager{ nullptr };
	GameCamera* camera{ nullptr };
	//stringstream consoleOutput;
	
public:
	Application();
	~Application();
	
	void run();
	Window* getWindow() { return window; }
	Camera* getCamera() { return camera; }

	InputManager* getInputManager() { return inputManager; }

	/*void flushConsoleOutput() { 
		//cout << consoleOutput.str(); 
		//consoleOutput.str("");//清空. ss不能用clear.
		
	}*/
};

extern Application* theApp;

#endif