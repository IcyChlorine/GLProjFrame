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