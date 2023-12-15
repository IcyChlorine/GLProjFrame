#include "Application.h"
#include "Mesh.h"

const int loglevel{DEFAULT_LOGLEVEL};

DemoApp::DemoApp()
{
	this->window = new Window((AbsObject*)this);

	// init GLAD
	// 要在创建窗口(glfw init)后再init glad，不然会出bug
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		error("Failed to initialize GLAD!\n");
		throw init_exception("Error: Failed to initialize GLAD");
	}

	glEnable(GL_MULTISAMPLE);

	// set viewport with respect to the window
	int width, height;
	window->getSize(&width, &height);
	glViewport(0, 0, width, height);

	// setup input manager
	this->inputManager = window->getInputManager();
	//input.init(window);

	// setup camera
	camera = new InspectCamera((AbsObject*)this);

	// 按ESC释放鼠标
	/*inputManager->addKeyCallback([&]() {
		camera->setCaptureCursor(!camera->getCaptureCursor());
		//cout << "key_released" << endl;
	}, GLFW_KEY_ESCAPE, InputManager::KEY_PRESS);*/
	
	// 显示帧率
	inputManager->addKeyCallback([&]() {
		show_hud = !show_hud;
	}, GLFW_KEY_F3, InputManager::KEY_PRESS);	

	// toggle fullscreen
	inputManager->addKeyCallback([&]() {
		window->toggleFullscreen();
	}, GLFW_KEY_F11, InputManager::KEY_PRESS);

	// quit on pressing Q
	inputManager->addKeyCallback([&]() {
		glfwSetWindowShouldClose(window->getInternalPointer(), true);
	}, GLFW_KEY_Q, InputManager::KEY_PRESS);

	inputManager->addKeyCallback([&]() {
		GameCamera* p = (GameCamera*)camera;
		p->accelerate = !p->accelerate;
	}, GLFW_KEY_LEFT_SHIFT, InputManager::KEY_PRESS);
}


DemoApp::~DemoApp()
{
	glfwTerminate();
	delete inputManager;
	delete camera;
}

//main loop
void DemoApp::run() {
	float dt{ 0.0f };//time passed(in sed) between frames
	float time,time_prev;//time counters
	
	unsigned long frame_cnt{ 0 };//走过的帧数

	float time_fps_updated; 
	unsigned long frame_cnt_prev{ 0 }; //计算fps用

	time = time_prev = time_fps_updated = glfwGetTime();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Text* text = new Text("<unknown>", glm::ivec2(0,0), 0.4f);
	Cube* cube = new Cube();
	cube->translate(glm::vec3(0.0f, 0.5f, 0.0f));
	ViewportGrid* grid = new ViewportGrid();

	double fps{0.0f};
	char hud_text[256];
	char* hud_printhead = hud_text;
	
	while (!window->shouldClose())
	{
		time = glfwGetTime();
		dt = time - time_prev; // Δt between frames
		time_prev = time;
		//--------------->Begin of Rendering Codes<----------------//
		// clearing
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		camera->update(dt);

		// RENDER!
		cube->render();
		grid->render();
		if(show_hud) text->render();
		
		//---------------->End of Rendering Codes<----------------//
		glfwSwapBuffers(window->getInternalPointer());
		glfwPollEvents();

		// update hud info
		if (time-time_fps_updated>0.3f) {//update console output per 0.8sec
			hud_printhead = hud_text;
			fps = (frame_cnt-frame_cnt_prev)*1.0f/(time-time_fps_updated);
			hud_printhead += sprintf(hud_printhead, "frame %d fps %d\n", frame_cnt, (int)fps);
			
			time_fps_updated = time;
			frame_cnt_prev = frame_cnt;
		}
		this->camera->outputDebugInfo(hud_printhead);
		text->setText(hud_text);

		frame_cnt++;
	}
}

MyApp::MyApp()
{
	this->window = new Window((AbsObject*)this);

	// init GLAD
	// 要在创建窗口(glfw init)后再init glad，不然会出bug
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		error("Failed to initialize GLAD!\n");
		throw init_exception("Error: Failed to initialize GLAD");
	}

	glEnable(GL_MULTISAMPLE);

	// set viewport with respect to the window
	int width, height;
	window->getSize(&width, &height);
	glViewport(0, 0, width, height);

	// setup input manager
	this->inputManager = window->getInputManager();
	//input.init(window);

	// setup camera
	camera = new GameCamera((AbsObject*)this);

	// 按ESC释放鼠标
	inputManager->addKeyCallback([&]() {
		camera->toggleCaptureCursor();
		//cout << "key_released" << endl;
	}, GLFW_KEY_ESCAPE, InputManager::KEY_PRESS);
	
	// 显示帧率
	inputManager->addKeyCallback([&]() {
		show_hud = !show_hud;
	}, GLFW_KEY_F3, InputManager::KEY_PRESS);	

	// toggle fullscreen
	inputManager->addKeyCallback([&]() {
		window->toggleFullscreen();
	}, GLFW_KEY_F11, InputManager::KEY_PRESS);

	// quit on pressing Q
	inputManager->addKeyCallback([&]() {
		glfwSetWindowShouldClose(window->getInternalPointer(), true);
	}, GLFW_KEY_Q, InputManager::KEY_PRESS);

	inputManager->addKeyCallback([&]() {
		line_mode = !line_mode;
	}, GLFW_KEY_L, InputManager::KEY_PRESS);	

	inputManager->addKeyCallback([&]() {
		GameCamera* p = (GameCamera*)camera;
		p->accelerate = !p->accelerate;
	}, GLFW_KEY_LEFT_SHIFT, InputManager::KEY_PRESS);
}


MyApp::~MyApp()
{
	glfwTerminate();
	delete inputManager;
	delete camera;
}

//main loop
void MyApp::run() {
	float dt{ 0.0f };//time passed(in sed) between frames
	float time,time_prev;//time counters
	
	unsigned long frame_cnt{ 0 };//走过的帧数

	float time_fps_updated; 
	unsigned long frame_cnt_prev{ 0 }; //计算fps用

	time = time_prev = time_fps_updated = glfwGetTime();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ColorfulStone* obj = new ColorfulStone();
	Text* text = new Text("<unknown>", glm::ivec2(0,0), 0.4f);
	Cube* cube = new Cube();
	ViewportGrid* grid = new ViewportGrid();

	//Model *model = new Model("assets/Lumine.obj");
	//Model *model = new Model("assets/HuTao/HuTao.pmx");
	//Model *model = new Model("assets/backpack/backpack.obj");
	//Model *model = new Model("assets/nanosuit/nanosuit.obj");
	//Model* model = new Model("assets/AstroMC/server_map_base.obj");
	/*Model* model = new Model("assets/Sponza/SponzaNoFlag.obj");
	model->scale(0.05f);
	model->scale(0.2f);
	model->translate(glm::vec3(0.0f, -1.0f, 0.0f));*/
	
	//model->translate(glm::vec3(1.0f, 0.0f, 0.0f));
	//model->rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//Cube* model = new Cube();

	double fps{0.0f};
	char hud_text[256];
	char* hud_printhead = hud_text;
	
	while (!window->shouldClose())
	{
		time = glfwGetTime();
		dt = time - time_prev; // Δt between frames
		time_prev = time;
		//--------------->Begin of Rendering Codes<----------------//
		// clearing
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		camera->update(dt);

		// RENDER!
		if(line_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//cube->render();
		//model->render();
		obj->render();
		grid->render();
		if(line_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
		if(show_hud)
			text->render();
		
		//---------------->End of Rendering Codes<----------------//
		glfwSwapBuffers(window->getInternalPointer());
		glfwPollEvents();

		// update hud info
		if (time-time_fps_updated>0.3f) {//update console output per 0.8sec
			hud_printhead = hud_text;
			fps = (frame_cnt-frame_cnt_prev)*1.0f/(time-time_fps_updated);
			hud_printhead += sprintf(hud_printhead, "frame %d fps %d\n", frame_cnt, (int)fps);
			
			time_fps_updated = time;
			frame_cnt_prev = frame_cnt;
		}
		this->camera->outputDebugInfo(hud_printhead);
		text->setText(hud_text);

		frame_cnt++;
	}
}