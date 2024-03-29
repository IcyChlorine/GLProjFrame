#pragma once
#ifndef _APP_H
#define _APP_H

#include "common_t1.h"
#include <sstream>

#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderable.h"
#include "Text.h"
#include "InputManager.h"
#include "GameCamera.h"

//#define interface class

// currently a very simple interface, with only one method
class GraphicsApplication {
private:
protected:
	// Is windows an essential?(there're applications that feartures off-screen rendering)
	Window* window{ nullptr }; 
	InputManager* inputManager{ nullptr };
public:
	// prevent copying, but still not a full-fledged singleton
	GraphicsApplication(const GraphicsApplication&) = delete;
	GraphicsApplication& operator=(const GraphicsApplication&) = delete;

	GraphicsApplication() {}
	virtual ~GraphicsApplication() {}
	virtual void run() = 0;

	virtual Window* getWindow() { return nullptr; }
	virtual InputManager* getInputManager() { return nullptr; }
	virtual Camera* getCamera() { return nullptr; }
};

class DemoApp : public GraphicsApplication {
private:
	bool show_hud{ false };

protected:
	Window* window{ nullptr };
	InputManager* inputManager{ nullptr };
	InspectCamera* camera{ nullptr };

public:
	DemoApp();
	~DemoApp();

	void run() override;
	Window* getWindow() override { return window; }
	InputManager* getInputManager() override { return inputManager; }
	Camera* getCamera() override { return camera; }
};

class MyApp : public GraphicsApplication
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
	MyApp();
	~MyApp();
	
	void run() override;
	Window* getWindow() override { return window; }
	Camera* getCamera() override { return camera; }

	InputManager* getInputManager() override { return inputManager; }

	/*void flushConsoleOutput() { 
		//cout << consoleOutput.str(); 
		//consoleOutput.str("");//清空. ss不能用clear.
		
	}*/
};

extern GraphicsApplication* theApp;

#endif