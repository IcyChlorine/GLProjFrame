#include "glpp/Application.h"

GraphicApplicationBase* GraphicApplicationBase::instance{nullptr};

GraphicApplicationBase::GraphicApplicationBase(){
	if(instance)
		throw init_exception("GraphicApplicationBase: Instance already exists!");
	else
		instance=this;

	Logging::info("INIT", "Initialiting glfw...");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	Logging::info("INIT","Initiating Success!");

	Logging::info("INIT", "Creating window via glfw...");
	this->window = new Window(this);
	//setup UserInput
	this->inputManager = window->getInputManager();
	//input.init(window);

	Logging::info("INIT","Initiating OpenGL via glad...");
	//init GLAD
	//要在创建窗口后再init glad，不然会出bug
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw init_exception("Error: Failed to initialize GLAD");
	Logging::info("INIT","Initiating Success!");

	//set viewport with respect to the window
	int width, height;
	window->getSize(&width, &height);
	glViewport(0, 0, width, height);
}
GraphicApplicationBase::~GraphicApplicationBase(){
	glfwTerminate();
}

Application::Application()
{
	//--------------->Begin of Rendering Init<----------------//
	
	//generate VBO/EBO presets via VAO
	//unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// generate VBO
	//unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// clearify FORMAT of vertices array
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//generate EBO
	//unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//generating shaders dynamically
	Logging::info("Loading shader(s)...");
	shader = new Shader("src\\shaders\\vertex.glsl", "src\\shaders\\frag.glsl");

	//load texture
	Logging::info("Loading texture(s)...");
	texture = new Texture("texture\\stone.bmp");

	//setup camera
	camera = new GameCamera(this);

	//鼠标操控旋转
	inputManager->registerMouseMoveCallback([&](float cursor_x, float cursor_y) {
		if(!this->dragging) return;

		int width, height;
		auto pWindow = window->getInternalPointer();
		glfwGetWindowSize(pWindow, &width, &height);

		float d_phi = -(cursor_x - this->prev_cursor_x) / width * 180;
		float d_th = (cursor_y - this->prev_cursor_y) / height * 90;

		camera->rotate(d_th, d_phi);

		this->prev_cursor_x=cursor_x;
		this->prev_cursor_y=cursor_y;
	});
	inputManager->registerMouseClickCallback([&](int key, int action){
		if(key==GLFW_MOUSE_BUTTON_LEFT){
			if(action==GLFW_PRESS){
				this->dragging=true;
				//reset prev_cursor_pos to avoid FLICKERING
				double tmp_x,tmp_y;
				glfwGetCursorPos(this->window->getInternalPointer(), &tmp_x, &tmp_y);
				this->prev_cursor_x=(float)tmp_x;
				this->prev_cursor_y=(float)tmp_y;
			}else if(action==GLFW_RELEASE){
				this->dragging=false;
			}
		}
	});
	
	inputManager->registerKeyCallback([&](int key, int action) {
		//按ESC释放鼠标
		//camera->setEnabled(!camera->getEnabled());
		if(action==GLFW_PRESS)
			this->key_pressed[key]=true;
		else if(action==GLFW_RELEASE)
			this->key_pressed[key]=false;
		if(action==GLFW_PRESS && key==GLFW_KEY_P){
			this->require_FPS_info = !this->require_FPS_info;
		}
	});
	//time_prev = (float)glfwGetTime();
	Logging::info("Profiling off by default. Press 'P' to switch fps info on/off.");
}


Application::~Application()
{
	delete shader;
	delete inputManager;
	delete camera;
}

//main loop
void Application::run() {

	float dt{ 0.0f };//time passed(in sed) between frames
	float time,time_prev,time_prev2;//time counters
	time = time_prev = time_prev2 = glfwGetTime();

	unsigned long frame_cnt{ 0 };//走过的帧数

	while (!window->shouldClose())
	{
		frame_cnt++;
		time = glfwGetTime();
		dt = time - time_prev;//calculate delta t between frames
		time_prev = time;
		//--------------->Simulation Codes<------------------------//
		auto dr_local = (dt*1.0f)*glm::vec3(
			(key_pressed[GLFW_KEY_D]?1.0f:0.0f)		-(key_pressed[GLFW_KEY_A]?1.0f:0.0f),
			(key_pressed[GLFW_KEY_SPACE]?1.0f:0.0f)	-(key_pressed[GLFW_KEY_CAPS_LOCK]?1.0f:0.0f),
			(key_pressed[GLFW_KEY_W]?1.0f:0.0f)		-(key_pressed[GLFW_KEY_S]?1.0f:0.0f)
		);
		camera->move(dr_local);
		//--------------->Begin of Rendering Codes<----------------//
		//clearing
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->use();
		texture->use();

		//camera->update(dt);
		camera->applyCameraTransform(*shader);

		//set perspective projection
		int width, height;
		window->getSize(&width, &height);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		Transform projTrans;
		projTrans.enable(true);
		projTrans.setTransformMat(proj);
		projTrans.apply(*shader,"proj");

		//RENDER!
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//---------------->End of Rendering Codes<----------------//
		glfwSwapBuffers(window->getInternalPointer());
		glfwPollEvents();

		//极其ugly的profiler，但勉强能用
		if (time-time_prev2>0.8f) {//update console output per 0.8sec
			if(require_FPS_info){
				cout << "-----frame " << frame_cnt << "-----" << endl;
				cout << "dt: " << dt * 1000 << "ms\n";
				this->camera->outputDebugInfo(cout);
				cout << "\n\n";
			}

			time_prev2 = time;
		}
		
		
	}
}