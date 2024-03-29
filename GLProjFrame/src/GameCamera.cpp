#include "GameCamera.h"
#include "Window.h"
#include "Application.h"

#include "glm/glm.hpp"

GameCamera::GameCamera(AbsObject* father) : Camera{ father } {
	int width, height;
	auto window = theApp->getWindow();
	window->getSize(&width, &height);
	InputManager* input = window->getInputManager();
	input->setCursorPos(width / 2, height / 2);
}

GameCamera::~GameCamera()
{ }

inline float clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

void GameCamera::placeCursorAtCenter() {
	auto input = theApp->getInputManager();
	int width, height;
	input->getWindowSize(width, height);
	input->setCursorPos(width / 2, height / 2);
}

void GameCamera::getDirectionVectors(glm::vec3& front, glm::vec3& up, glm::vec3& right) {
	front = glm::vec3(
		sin(glm::radians(th)) * sin(glm::radians(phi)),
		cos(glm::radians(th)),
		sin(glm::radians(th)) * cos(glm::radians(phi))
	);
	right = glm::vec3(
		sin(glm::radians(phi-90)),
		0,
		cos(glm::radians(phi-90))
	);
	up = glm::cross(right, front);
}

void GameCamera::setCaptureCursor(bool capture_cursor) {
	this->capture_cursor = capture_cursor;
	if (capture_cursor) {
		placeCursorAtCenter();
	}
}

void GameCamera::update(float dt)
{
	cumulated_time += dt;
	// 只有时间间隔足够大时才更新变换，避免因鼠标只移动了一两个像素造成的不平滑
	// Extra notes: if vsync is turned of, this is not necessay
	//if (cumulated_time < 0.01f)
	//	return;

	// get these handy vectors
	glm::vec3 front, right, up;
	this->getDirectionVectors(front, up, right);
	
	// 鼠标控制视角旋转
	InputManager* input = theApp->getInputManager();
	int width, height;
	float cursor_x, cursor_y;
	input->getWindowSize(width, height);
	input->getCursorPos(cursor_x, cursor_y);

	if(capture_cursor) {
		phi-= (cursor_x - width / 2)*phi_rate * 360;
		th += (cursor_y - height / 2)*th_rate * 90;
		th  = clamp(th, 0.0f, 180.0f);

		input->setCursorPos(width / 2, height / 2);
	}

	// WASD控制平移
	glm::vec4 dr = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (input->isKeyPressed(GLFW_KEY_D))
		dr.x += dt*vx;
	if (input->isKeyPressed(GLFW_KEY_A))
		dr.x -= dt*vx;
	if (input->isKeyPressed(GLFW_KEY_S))
		dr.z -= dt*vz;
	if (input->isKeyPressed(GLFW_KEY_W))
		dr.z += dt*vz;
	if (input->isKeyPressed(GLFW_KEY_SPACE))
		dr.y += dt*vy;
	if (input->isKeyPressed(GLFW_KEY_CAPS_LOCK))
		dr.y -= dt*vy;
	if(accelerate) dr *= 3;

	// project to y axis, and xz plane, respectively.
	glm::vec3 mv_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 mv_front = glm::vec3(sin(glm::radians(phi)), 0.0f, cos(glm::radians(phi)));

	this->pos += dr.x*right + dr.y*mv_up + dr.z*mv_front;

	// 计算变换
	/*
	 * Notes: Matrix multiplication related to OpenGL is quite tricky. Here's why:
	 *   First of all, OpenGL uses *column-major* matrix, which is different from C/C++.
	 *   immediate mode OpenGL(old versions of OpenGL), glsl, glm all follows column-major
	 *   convention. So mat[1] would get the second column of matrix mat(both in glm and glsl),
	 *   not the second row as in C/C++. 
	 *   This explains the fact that when you try to print a glm::mat4 transMat using
	 *   traditional C/C++ methods, you get a (seemingly incorrect) transposed
	 *   view. Fortunately, glm and glsl follow the same convention, so no need to worry
	 *   that at most times.
	 * 
	 *   Second, glm::translate|rotate|scale|... functions as if they're *right-multiplying*
	 *   transform matrices to the original matrices, not left-multiplying, as we're taught
	 *   in linear algebra classes.
	 *   Such design(right-multi) makes it intuitive to manipulate a model: you translate it(glm::translate) first,
	 *   rotate it(glm::rotate) at the translated position, then scale it(again in place).
	 *   In other words, all the transforms are applied in the local coordinate system.
	 *   Otherwise(left multi), you'll be operating in global coord. and have to apply the 
	 *   transforms in reverse order.
	 *   Old OpenGL defines glTranslate|glRotate|glScale|glMultMatrix as aforementioned(right-multi),
	 *   and glm has been designed to agree with OpenGL conventions, so it's right-multi too.
	 *   (see https://docs.gl/gl3/glMultMatrix for a clear example)
	 * 
	 *   In summary, glm::translate|rotate|scale|... are designed to manipulate models, or
	 *   in local coordinates. When applied for camera, be aware of it's right-multi-ness.
	 *   Or actually, glm::lookAt will be more handy for camera/generating view matrix.
	*/

	/*
	auto transMat = glm::mat4(1.0f);
	transMat = glm::rotate(transMat, glm::radians( th - 90), glm::vec3(1.0f, 0.0f, 0.0f));
	transMat = glm::rotate(transMat, glm::radians(-phi-180), glm::vec3(0.0f, 1.0f, 0.0f));
	transMat = glm::translate(transMat, -this->pos);
	*/
	glm::mat4 transMat = glm::lookAt(pos, pos + front, up);
	view = transMat; 

	// 透视投影变换
	// TODO: make these parameters adjustable
	float vertical_fov = 45.0f;
	float zNear = 0.1f, zFar = 100.0f;
	proj = glm::perspective(glm::radians(vertical_fov), (float)width / (float)height, zNear, zFar);
	//print_glm_mat4(cout, proj);
}

void GameCamera::outputDebugInfo(ostream & out)
{
	char str[100];
	sprintf(str, "(x, y, z) = (%.2f, %.2f, %.2f)\n(th, phi)=(%.2f, %.2f)\n", 
		pos.x, pos.y, pos.z, th, phi);
	out << str;
}

int GameCamera::outputDebugInfo(char* buf)
{
	return sprintf(buf, "(x,y,z) = (%.2f,%.2f,%.2f)\n(th, phi) = (%.2f,%.2f)\n", 
		pos.x, pos.y, pos.z, th, phi);
}

InspectCamera::InspectCamera(AbsObject* father) : Camera{ father } {
	int width, height;
	auto input_manager = theApp->getInputManager();
	input_manager->getWindowSize(width, height);

	input_manager->addMouseClickCallback([&, father]() {
		// Note: Don't capture local variables by reference, as this function will
		// work as callback, and the local variables will be destroyed when called.
		dragging = true;
		auto input = theApp->getInputManager();
		drag_start = input->getCursorPos();
		drag_start_th = th;
		drag_start_phi = phi;
	}, 0, 0);
	input_manager->addMouseClickCallback([&]() {
		dragging = false;
	}, 0, 1);
	
	// zoom up dist when mousewheel
	input_manager->addMouseScrollCallback([&](double yoffset) {
		dist *= exp(-yoffset*0.1); // scale invariant
	});

	this->update(0);
}

InspectCamera::~InspectCamera()
{ }

// convert spherical coordinates to cartesian coordinates
inline glm::vec3 angle2vec(float th, float phi) {
	return glm::vec3(
		sin(glm::radians(th)) * sin(glm::radians(phi)),
		cos(glm::radians(th)),
		sin(glm::radians(th)) * cos(glm::radians(phi))
	);
}

void InspectCamera::getDirectionVectors(glm::vec3& front, glm::vec3& up, glm::vec3& right) {
	front = glm::vec3(
		sin(glm::radians(th)) * sin(glm::radians(phi)),
		cos(glm::radians(th)),
		sin(glm::radians(th)) * cos(glm::radians(phi))
	);
	right = glm::vec3(
		sin(glm::radians(phi-90)),
		0,
		cos(glm::radians(phi-90))
	);
	up = glm::cross(right, front);
}

void InspectCamera::update(float dt)
{
	// these vectors are handy
	glm::vec3 front, right, up;
	this->getDirectionVectors(front, up, right);
	
	int width, height;
	InputManager* input = theApp->getInputManager();
	input->getWindowSize(width, height);

	if(this->dragging) {
		// 鼠标控制视角旋转
		
		float cursor_x, cursor_y;
		input->getCursorPos(cursor_x, cursor_y);
	
		//phi += (time - time_prev)*10;
		phi = drag_start_phi - (cursor_x - drag_start.x)*phi_rate * 360;
		th  = drag_start_th  + (cursor_y - drag_start.y)*th_rate * 90;
		th  = clamp(th, 0.0f, 180.0f);
	}

	// WASD控制平移
	glm::vec3 dr = glm::vec4(0.0f);
	if (input->isKeyPressed(GLFW_KEY_D))
		dr.x += dt*vx;
	if (input->isKeyPressed(GLFW_KEY_A))
		dr.x -= dt*vx;
	if (input->isKeyPressed(GLFW_KEY_S))
		dr.z -= dt*vz;
	if (input->isKeyPressed(GLFW_KEY_W))
		dr.z += dt*vz;
	if (input->isKeyPressed(GLFW_KEY_SPACE))
		dr.y += dt*vy;
	if (input->isKeyPressed(GLFW_KEY_CAPS_LOCK))
		dr.y -= dt*vy;
	if(accelerate) dr *= 3;

	// project to y axis, and xz plane, respectively.
	glm::vec3 mv_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 mv_front = glm::vec3(sin(glm::radians(phi)), 0.0f, cos(glm::radians(phi)));

	this->focus += dr.x*right + dr.y*mv_up + dr.z*mv_front;

	// 计算变换
	/*
	auto transMat = glm::mat4(1.0f);
	transMat = glm::rotate(transMat, glm::radians( th - 90), glm::vec3(1.0f, 0.0f, 0.0f));
	transMat = glm::rotate(transMat, glm::radians(-phi-180), glm::vec3(0.0f, 1.0f, 0.0f));
	transMat = glm::translate(transMat, -this->pos);
	*/
	glm::mat4 transMat = glm::lookAt(focus-dist*front, focus, up);
	view = transMat; 

	// 透视投影变换
	// TODO: make these parameters adjustable
	float vertical_fov = 45.0f;
	float zNear = 0.1f, zFar = 100.0f;
	proj = glm::perspective(glm::radians(vertical_fov), (float)width / (float)height, zNear, zFar);
	//print_glm_mat4(cout, proj);
}

void InspectCamera::outputDebugInfo(ostream & out)
{
	//TODO
	char str[100];
	sprintf(str, "focusing (x, y, z) = (%.2f, %.2f, %.2f)\n(th, phi)=(%.2f, %.2f)\n", 
		focus.x, focus.y, focus.z, th, phi);
	out << str;
}

int InspectCamera::outputDebugInfo(char* buf)
{
	return sprintf(buf, "focusing (x,y,z) = (%.2f,%.2f,%.2f)\n(th, phi) = (%.2f,%.2f)\n", 
		focus.x, focus.y, focus.z, th, phi);
}
