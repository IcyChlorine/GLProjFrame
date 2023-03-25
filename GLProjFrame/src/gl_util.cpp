#include "gl_util.h"

// 不是inline，不能放在头文件里！否则会造成多定义错误。
void declare_interleaving_vert_data_layout(int k, int* p) {
	int tot_cnt {0};
	for(int i=0; i<k; i++){
		tot_cnt += *(p+i);
	}
	int offset{0};
	for(int i=0; i<k; i++){
		glVertexAttribPointer(
			i, *(p+i), // nr, size
			GL_FLOAT, GL_FALSE, // some almost fixed parameter
			tot_cnt * sizeof(float), // stride
			(void*) (offset * sizeof(float)) // offset
		);
		offset += *(p+i);
		glEnableVertexAttribArray(i);
	}
}

std::stack<bool> depth_stack;
std::stack<bool> blend_stack;

void set_and_push_depth_test_status(bool enabled) {
	depth_stack.push(glIsEnabled(GL_DEPTH_TEST));
	if(enabled) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void pop_and_resume_depth_test_status() {
	if(depth_stack.empty()) {
		//深度测试的默认值：启用
		glEnable(GL_DEPTH_TEST);
		return;
	}

	auto enabled = depth_stack.top();
	depth_stack.pop();
	if(enabled) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void set_and_push_blend_status(bool enabled) {
	depth_stack.push(glIsEnabled(GL_BLEND));
	if(enabled) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}

void pop_and_resume_blend_status() {
	if(depth_stack.empty()) {
		//blend的默认值：启用
		glEnable(GL_BLEND);
		return;
	}

	auto enabled = depth_stack.top();
	depth_stack.pop();
	if(enabled) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}