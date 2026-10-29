#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <gl/glew.h>



inline bool CompileShader(GLuint shader, const char* file_name) {
	std::ifstream vertex_shader_file{ file_name };
	if (!vertex_shader_file) {
		std::cerr << "Unable to open shader file: " << file_name << std::endl;
		return false;
	}
	std::stringstream buf;
	buf << vertex_shader_file.rdbuf();

	std::string shader_source = buf.str();
	auto source_ptr = shader_source.c_str();

	glShaderSource(shader, 1, &source_ptr, 0);
	glCompileShader(shader);

	GLint is_success = GL_FALSE;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_success);
	if (!is_success) {
		GLint log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		std::vector<GLchar> error_message(static_cast<std::size_t>(std::max(log_length, 1)));
		glGetShaderInfoLog(shader, log_length, nullptr, error_message.data());
		std::cerr << "Failed compiling shader " << file_name << ": "
			<< error_message.data() << std::endl;
	}

	return is_success;
}

inline bool InitShader(GLuint& shader_program) {
	auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	if (!CompileShader(vertex_shader, "vertex2.glsl")) {
		glDeleteShader(vertex_shader);
		return false;
	}

	auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!CompileShader(fragment_shader, "fragment2.glsl")) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return false;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint is_success = GL_FALSE;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
	if (!is_success) {
		GLint log_length = 0;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
		std::vector<GLchar> error_message(static_cast<std::size_t>(std::max(log_length, 1)));
		glGetProgramInfoLog(shader_program, log_length, nullptr, error_message.data());
		std::cerr << "Failed linking shader program: " << error_message.data() << std::endl;
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(shader_program);
		shader_program = 0;
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glUseProgram(shader_program);

	return true;
}
