#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "floor.h"


Floor::Floor(const glm::vec3 pos, const float w, const float d, const glm::vec3 col)
	: position(pos), width(w), depth(d), color(col)
{
}

Floor::~Floor()
{
}

glm::vec3 Floor::getPos() {
	return position;
}

void Floor::initBuffer(GLuint shader_program) {
	GLfloat vFloorPosList[] = {
			-0.5f, 0.f, -0.5f,
			0.5f, 0.f, -0.5f,
			0.5f, 0.f, 0.5f,
			-0.5f, 0.f, 0.5f
	};

	unsigned int floorIndex[] = {
		0,3,2,
		0,2,1
	};

	GLfloat floorNormal[] = {
		0.f, 1.f, 0.f,		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,		0.f, 1.f, 0.f

	};

	glGenVertexArrays(1, &floor_buffer.vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(floor_buffer.vao); //--- VAO를 바인드하기
	glGenBuffers(2, floor_buffer.vbo);

	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vFloorPosList), vFloorPosList, GL_STATIC_DRAW);
	glGenBuffers(1, &floor_buffer.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_buffer.EBO); // GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndex), floorIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer.vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorNormal), floorNormal, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

}

void Floor::Draw(GLuint shader_program) {
	glm::mat4 transform(1.f);
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::radians(y_degree), glm::vec3(0, 1., 0));
	transform = glm::translate(transform, glm::vec3(z_offset,0,0));
	transform = glm::scale(transform, glm::vec3(width, 1.f, depth));
	GLint location = glGetUniformLocation(shader_program, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	int objColorLocation = glGetUniformLocation(shader_program, "objectColor");
	glUniform3f(objColorLocation, color.x, color.y, color.z);


	glBindVertexArray(floor_buffer.vao);
	glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, 0);
}

void Floor::set_state(glm::vec3 pos, float y_d, float o)
{
	z_offset = o;
	position = pos;
	y_degree = y_d;
}

void Floor::change_col(float i)
{
	color.x = i;
	color.y = i;
	color.z = i;
}
