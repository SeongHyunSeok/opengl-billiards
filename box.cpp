#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "box.h"


Box::Box(const glm::vec3 pos, const float w, const float d, const float h, const float y_d,const glm::vec3 col)
	: position(pos), width(w), depth(d), height(h), y_degree(y_d) ,color(col)
{
}

Box::~Box()
{
}



void Box::initBuffer(GLuint shader_program) {
	GLfloat boxNormal[] = {
		0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f,
		0.f, 0.f, 1.f,  0.f, 0.f, 1.f,  0.f, 0.f, 1.f,  0.f, 0.f, 1.f,
		-1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,  1.f, 0.f, 0.f,  1.f, 0.f, 0.f,  1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,  0.f, 1.f, 0.f,  0.f, 1.f, 0.f,  0.f, 1.f, 0.f,
		0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f
	};

	GLfloat vBoxPosList[] = {
		-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,   -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,   0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,  0.5f, -0.5f, 0.5f,   0.5f, 0.5f, 0.5f,    0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,   0.5f, 0.5f, -0.5f,   0.5f, 0.5f, 0.5f,   -0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,   0.5f, -0.5f, 0.5f,   0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f
	};

	unsigned int boxIndex[] =
	{
		0,1,2, 0,2,3,
		4,5,6, 4,6,7,
		8,9,10, 8,10,11,
		12,13,14, 12,14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23
	};


	glGenVertexArrays(1, &box_buffer.vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(box_buffer.vao); //--- VAO를 바인드하기
	glGenBuffers(2, box_buffer.vbo);

	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, box_buffer.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vBoxPosList), vBoxPosList, GL_STATIC_DRAW);
	glGenBuffers(1, &box_buffer.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_buffer.EBO); // GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndex), boxIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, box_buffer.vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxNormal), boxNormal, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);
}

void Box::Draw(GLuint shader_program) {
	glm::mat4 transform(1.f);
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::radians(y_degree), glm::vec3(0, 1., 0));
	transform = glm::scale(transform, glm::vec3(width, height, depth));
	

	GLint location = glGetUniformLocation(shader_program, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	int objColorLocation = glGetUniformLocation(shader_program, "objectColor");
	glUniform3f(objColorLocation, color.x, color.y, color.z);


	glBindVertexArray(box_buffer.vao);
	glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0);
}

void Box::set_state(glm::vec3 new_position, float new_y_degree) {
	position = new_position;
	y_degree = new_y_degree;
}

glm::vec3 Box::getPos() {
	return position;
}

float Box::getY_Deg() {
	return y_degree;
}

float Box::getW() {
	return width;
}
float Box::getD() {
	return depth;
}

