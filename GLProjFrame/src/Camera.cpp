#include "Camera.h"

Camera::Camera() { }

Camera::Camera(AbsObject * father):
	AbsObject{father} { }


Camera::~Camera() { }

void Camera::applyCameraTransform(Shader& shader)
{
	shader.setUniformMatrix("view", this->view);
	shader.setUniformMatrix("proj", this->proj);
}


void print_glm_mat4(std::ostream& out, glm::mat4 mat) {
    char buffer[100];
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
			// Note: OpenGL and all related stuff(glsl, glm) use *column-major* matrix!
			//       This is drastically different from the convention in C/C++. Be aware!
            sprintf(buffer, "%.2f ", mat[c][r]);
            out << buffer;
        }
        out << "\n";
    }
}