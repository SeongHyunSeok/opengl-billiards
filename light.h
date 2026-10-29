#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <glm/glm.hpp>
#include "shader.h"

inline void light_on(glm::vec3 pos, GLuint shader_program) {
	int lightPosLocation = glGetUniformLocation(shader_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, pos.x, pos.y, pos.z);
	int lightColorLocation = glGetUniformLocation(shader_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1.f, 1.f, 1.f);
}
