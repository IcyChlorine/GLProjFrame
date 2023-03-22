#include "Application.h"

const int loglevel{DEFAULT_LOGLEVEL};

Application::Application()
{
	this->window = new Window(this);

	//init GLAD
	//要在创建窗口(glfw init)后再init glad，不然会出bug
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		error("Failed to initialize GLAD!\n");
		throw init_exception("Error: Failed to initialize GLAD");
	}

	//set viewport with respect to the window
	int width, height;
	window->getSize(&width, &height);
	glViewport(0, 0, width, height);

	//generating shaders dynamically
	shader = new Shader("src\\shaders\\vertex.glsl", "src\\shaders\\frag.glsl");
	

	//load texture
	//texture = new Texture("texture\\stone.bmp");

	//setup UserInput
	this->inputManager = window->getInputManager();
	//input.init(window);

	//setup camera
	camera = new GameCamera(this);

	//按ESC释放鼠标
	inputManager->setKeyCallback([&]() {
		camera->setEnabled(!camera->getEnabled());
		//cout << "key_released" << endl;
	}, GLFW_KEY_ESCAPE, 0);
	
	// 显示帧率
	inputManager->setKeyCallback([&]() {
		show_hud = !show_hud;
	}, GLFW_KEY_F3, 0);	

	inputManager->setKeyCallback([&]() {
		line_mode = !line_mode;
	}, GLFW_KEY_L, 0);	

	auto t = Text();
}


Application::~Application()
{
	glfwTerminate();
	delete shader;
	delete inputManager;
	delete camera;
}

//main loop
void Application::run() {
	float dt{ 0.0f };//time passed(in sed) between frames
	float time,time_prev;//time counters
	
	unsigned long frame_cnt{ 0 };//走过的帧数

	float time_prev2; 
	unsigned long frame_cnt_prev{ 0 }; //计算fps用

	time = time_prev = time_prev2 = glfwGetTime();

	Transform proj;
	proj.enable(true);

	ColorfulStone* obj = new ColorfulStone();
	obj->setShader(shader);
	Text* text = new Text("<unknown>", glm::ivec2(0,0), 0.4f);
	while (!window->shouldClose())
	{
		frame_cnt++;
		time = glfwGetTime();
		dt = time - time_prev;//calculate delta t between frames
		time_prev = time;
		//--------------->Begin of Rendering Codes<----------------//
		//clearing
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		shader->use(); // note that transform uniforms are set for the CURRENT shader
		camera->update(dt);
		camera->applyCameraTransform(*shader);
		
		//set perspective projection
		int width, height;
		window->getSize(&width, &height);
		proj.setTransformMat(glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f));
		proj.apply(*shader,"proj");

		//RENDER!
		if(line_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		obj->render();
		if(line_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if(show_hud)
			text->render();
		
		//---------------->End of Rendering Codes<----------------//
		glfwSwapBuffers(window->getInternalPointer());
		glfwPollEvents();

		// update hud info
		if (time-time_prev2>0.3f) {//update console output per 0.8sec
			char hud_text[256]; char* p = hud_text;

			int fps = (frame_cnt-frame_cnt_prev)*1.0f/(time-time_prev2);
			p += sprintf(p, "frame %d fps %d\n", frame_cnt, fps);
			p += this->camera->outputDebugInfo(p);
			text->setText(hud_text);

			time_prev2 = time;
			frame_cnt_prev = frame_cnt;
		}
	}
}