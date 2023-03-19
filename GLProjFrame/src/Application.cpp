#include "Application.h"



Application::Application()
{

	this->window = new Window(this);

	//init GLAD
	//要在创建窗口后再init glad，不然会出bug
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw init_exception("Error: Failed to initialize GLAD");

	//set viewport with respect to the window
	int width, height;
	window->getSize(&width, &height);
	glViewport(0, 0, width, height);

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
	shader = new Shader("src\\shaders\\vertex.glsl", "src\\shaders\\frag.glsl");

	//load texture
	texture = new Texture("texture\\stone.bmp");

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

	//time_prev = (float)glfwGetTime();
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
	float time,time_prev,time_prev2;//time counters
	time = time_prev = time_prev2 = glfwGetTime();

	unsigned long frame_cnt{ 0 };//走过的帧数

	while (!window->shouldClose())
	{
		frame_cnt++;
		time = glfwGetTime();
		dt = time - time_prev;//calculate delta t between frames
		time_prev = time;
		//--------------->Begin of Rendering Codes<----------------//
		//clearing
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->use();
		texture->use();

		camera->update(dt);
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
		if (time-time_prev2>0.3f) {//update console output per 0.8sec
			cout << "-----frame " << frame_cnt << "-----" << endl;
			cout << "dt: " << dt * 1000 << "ms\n";
			this->camera->outputDebugInfo(cout);
			cout << "\n\n";

			time_prev2 = time;
			//this->flushConsoleOutput();
		}
		
		
	}
}