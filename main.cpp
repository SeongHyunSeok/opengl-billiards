#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "shader.h"
#include "light.h"
#include "ball.h"
#include "box.h"
#include "floor.h"
#include "billiards.h"
#include <algorithm>
#include <chrono>
#include <filesystem>


GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid SpecialKeyboardUp(int key, int x, int y);
GLvoid TimerFunction(int value);

void InitBuffer();

int screen_w = 800;
int screen_h = 600;

GLuint shader_program;
//GLchar* vertexsource, * fragmentsource; //--- 소스코드 저장 변수
//GLuint vertexshader, fragmentshader; //--- 세이더 객체



int main(int argc, char** argv) //윈도우 출력하고 콜백함수 설정
{
	wchar_t executable_path[32768];
	const DWORD path_length = GetModuleFileNameW(nullptr, executable_path, 32768);
	if (path_length == 0 || path_length >= 32768) {
		std::cerr << "Unable to locate the executable directory." << std::endl;
		return EXIT_FAILURE;
	}
	const auto executable_dir = std::filesystem::path(executable_path).parent_path();
	// Packaged builds keep assets next to the executable; legacy VS builds use the source folder.
	if (std::filesystem::exists(executable_dir / "vertex2.glsl"))
		std::filesystem::current_path(executable_dir);
	for (const char* asset : { "vertex2.glsl", "fragment2.glsl", "sphere.obj", "billiard.wav" }) {
		if (!std::filesystem::exists(asset)) {
			std::cerr << "Required asset is missing: " << asset << std::endl;
			return EXIT_FAILURE;
		}
	}

	glutInit(&argc, argv);// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	const int monitor_width = glutGet(GLUT_SCREEN_WIDTH);
	const int monitor_height = glutGet(GLUT_SCREEN_HEIGHT);
	screen_w = std::max(1, static_cast<int>(monitor_width * 0.9f));
	screen_h = std::max(1, static_cast<int>(monitor_height * 0.9f));
	glutInitWindowPosition(
		std::max(0, (monitor_width - screen_w) / 2),
		std::max(0, (monitor_height - screen_h) / 2)
	); // 모니터 중앙에 창 배치
	glutInitWindowSize(screen_w, screen_h); // 윈도우의 크기 지정
	glutCreateWindow("Project"); // 윈도우 생성 윈도우 이름

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)// glew 초기화
	{
		std::cout << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized \n";

	glEnable(GL_DEPTH_TEST);
	
	if (!GLEW_VERSION_3_3) {
		std::cerr << "OpenGL 3.3 or later is required." << std::endl;
		return EXIT_FAILURE;
	}
	if (!InitShader(shader_program))
		return EXIT_FAILURE;
	InitBuffer();

	glutDisplayFunc(DrawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape);// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutSpecialUpFunc(SpecialKeyboardUp);
	glutIgnoreKeyRepeat(1);
	glutTimerFunc(16, TimerFunction, 1);



	glutMainLoop();// 이벤트 처리 시작
}


void InitBuffer()
{
	init_buf(shader_program);
}

void DrawScene() {

	//--- 변경된 배경색 설정
	glClearColor(0.53f, 0.81f, 1.0f, 1.0f);
	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_w, screen_h);

	draw_view(shader_program);
	
	GLint projTransform = glGetUniformLocation(shader_program, "projTransform");
	glm::mat4 projMat = glm::perspective(glm::radians(45.f), (float)screen_w / (float)screen_h, 0.1f, 10000.f);
	glUniformMatrix4fv(projTransform, 1, GL_FALSE, glm::value_ptr(projMat));

	glm::vec3 light_pos(0.f, 200.f, 0.f);
	light_on(light_pos, shader_program);

	
	draw_world(shader_program);

	// 평면도
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		const float minimap_aspect =
			(board_depth + box_width * 2.f) / (board_width + box_width * 2.f);
		const int map_width = std::max(1, static_cast<int>(screen_w * 0.42f));
		const int map_height = std::max(1, static_cast<int>(map_width / minimap_aspect));
		const int map_offset_x = static_cast<int>(map_width * 0.23f);
		const int map_offset_y = static_cast<int>(map_height * 0.23f);
		glViewport(
			screen_w - map_width + map_offset_x,
			screen_h - map_height + map_offset_y,
			map_width,
			map_height
		);

		GLint viewTransform = glGetUniformLocation(shader_program, "viewTransform");
		glm::mat4 viewMat = glm::lookAt(
			glm::vec3(0.f, 350.f, 0.f),
			glm::vec3(0.f),
			glm::vec3(1.f, 0.f, 0.f)
		);
		glUniformMatrix4fv(viewTransform, 1, GL_FALSE, glm::value_ptr(viewMat));

		GLint projTransform = glGetUniformLocation(shader_program, "projTransform");
		glm::mat4 projMat = glm::perspective(
			glm::radians(45.f),
			static_cast<float>(map_width) / static_cast<float>(map_height),
			0.1f,
			10000.f
		);

		glUniformMatrix4fv(projTransform, 1, GL_FALSE, glm::value_ptr(projMat));

		draw_world(shader_program);
	}

	glutSwapBuffers(); //--- 화면에 출력하기
}



void Reshape(int w, int h) {
	screen_w = w;
	screen_h = (h > 0) ? h : 1;
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{

	Key_input(key, x, y);
	glutPostRedisplay();
}

GLvoid SpecialKeyboard(int key, int x, int y) {
	
	special_Key(key, x, y);
	glutPostRedisplay();
}

GLvoid SpecialKeyboardUp(int key, int x, int y) {
	special_Key_up(key, x, y);
	glutPostRedisplay();
}

GLvoid TimerFunction(int value)
{
	using Clock = std::chrono::steady_clock;
	static auto previous_time = Clock::now();
	const auto current_time = Clock::now();
	const float delta_time = std::chrono::duration<float>(current_time - previous_time).count();
	previous_time = current_time;

	TimerFunc(std::clamp(delta_time, 0.f, 0.05f));
	glutPostRedisplay();// 화면 재 출력
	glutTimerFunc(16, TimerFunction, value);// 약 60Hz로 타이머함수 재 설정

}
