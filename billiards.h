#pragma once
#include <algorithm>
#include <cmath>
#include <limits>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <Windows.h>
#include <mmsystem.h>
#include "shader.h"
#include "ball.h"
#include "box.h"
#include "floor.h"
#include "physics.h"
#include "collision.h"

#define SPACEBAR 32


void init_buf(GLuint);
void draw_world(GLuint);
void special_Key(int key, int x, int y);
void special_Key_up(int key, int x, int y);
void Key_input(unsigned char key, int x, int y);
void TimerFunc(float delta_time);

#define _CRT_SECURE_NO_WARNINGS

#define PIXEL_PER_METER 1  // 10 pixel 30 cm


float ball_radius_meter = 12.6f / 2.f;


float board_width = 124.4f * PIXEL_PER_METER;

float board_depth = 244.8f * PIXEL_PER_METER;

float point_distance = 30.6f * PIXEL_PER_METER;

float box_width = 10 * PIXEL_PER_METER;

float box_height = 10 * PIXEL_PER_METER;

// Table dimensions are in centimeters, so gravity is expressed in cm/s^2.
float gravity = 980.f * PIXEL_PER_METER;

float coefficient_of_friction = 0.035f;

// Maximum cue impulse in N*s. The velocity conversion is handled in physics.h.
float maximum_cue_impulse = 0.8f;

float ball_mass = 0.21f; // kg

float ball_restitution = 0.93f;
float cushion_restitution = 0.80f;

bool player_1_turn = true;

// board
glm::vec3 floor_pos(0.f, 0.f, 0.f);

glm::vec3 floor_col(0.f, 0.81f, 0.82f);

Floor billard_floor(floor_pos, board_width, board_depth, floor_col);

Floor gage_bar(floor_pos, 2.f, 2.f, glm::vec3(1.f, 0.35f, 0.02f));
Floor secondary_guide_dot(floor_pos, 2.f, 2.f, glm::vec3(1.f, 0.1f, 0.7f));

// box
glm::vec3 box_col(0.55f, 0.27f, 0.07f);
// left
glm::vec3 left_box_pos(board_width / 2 + box_width / 2, 0.f, 0.f);

Box right_box(left_box_pos, box_width, board_depth + box_width * 2, box_height, 0.f, box_col);
// top
glm::vec3 top_box_pos(0.f, 0.f, -(board_depth / 2 + box_width / 2));

Box top_box(top_box_pos, box_width, board_width + box_width * 2, box_height, -90.f, box_col);

// right
glm::vec3 right_box_pos(-(board_width / 2 + box_width / 2), 0.f, 0.f);

Box left_box(right_box_pos, box_width, board_depth + box_width * 2, box_height, 180.f, box_col);

// bottom
glm::vec3 bottom_box_pos(0.f, 0.f, (board_depth / 2 + box_width / 2));

Box bottom_box(bottom_box_pos, box_width, board_width + box_width * 2, box_height, 90.f, box_col);

constexpr float cue_length = 80.f;
constexpr float cue_pull_distance = 35.f;
constexpr float cue_rest_gap = 3.f;
constexpr float cue_forward_speed = 180.f;
Box cue_stick(
	glm::vec3(0.f),
	1.2f,
	cue_length,
	1.2f,
	0.f,
	glm::vec3(0.72f, 0.42f, 0.18f)
);


// ball
glm::vec3 sub_ball_col(1.f, 0.0f, 0.0f);
glm::vec3 main1_ball_col(1.f, 1.f, 1.f);
glm::vec3 main2_ball_col(1.f, 0.65f, 0.f);

// main 1
glm::vec3 main_ball_1_pos(0.f, ball_radius_meter, -(point_distance * 3));

Ball main_ball_1(main_ball_1_pos, ball_radius_meter, ball_mass, main1_ball_col);

// main 2
glm::vec3 main_ball_2_pos(-point_distance, ball_radius_meter, point_distance * 2);

Ball main_ball_2(main_ball_2_pos, ball_radius_meter, ball_mass, main2_ball_col);

// sub
glm::vec3 sub_ball_1_pos(0.f, ball_radius_meter, -(point_distance * 2));

Ball sub_ball_1(sub_ball_1_pos, ball_radius_meter, ball_mass, sub_ball_col);

glm::vec3 sub_ball_2_pos(0.f, ball_radius_meter, (point_distance * 2));

Ball sub_ball_2(sub_ball_2_pos, ball_radius_meter, ball_mass, sub_ball_col);


void init_buf(GLuint shader_program)
{
	billard_floor.initBuffer(shader_program);
	gage_bar.initBuffer(shader_program);
	secondary_guide_dot.initBuffer(shader_program);

	left_box.initBuffer(shader_program);
	top_box.initBuffer(shader_program);
	right_box.initBuffer(shader_program);
	bottom_box.initBuffer(shader_program);
	cue_stick.initBuffer(shader_program);

	main_ball_1.initBuffer(shader_program);
	main_ball_2.initBuffer(shader_program);
	sub_ball_1.initBuffer(shader_program);
	sub_ball_2.initBuffer(shader_program);
}

float force_y_degree = 90;

int view_num = 0;
enum class CameraMode {
	Player,
	Ball
};
CameraMode camera_mode = CameraMode::Player;
bool camera_locked_during_shot = false;
glm::vec3 locked_camera_focus(0.f);
float locked_camera_heading = 90.f;

void draw_view(GLuint shader_program) {
	glm::vec3 p1;
	const bool use_locked_player_camera =
		camera_mode == CameraMode::Player && camera_locked_during_shot;

	if (use_locked_player_camera) {
		p1 = locked_camera_focus;
	}
	else if (view_num == 0) {
		p1 = main_ball_1.getPos();
	}
	else {
		p1 = main_ball_2.getPos();
	}
	const float camera_heading = use_locked_player_camera
		? locked_camera_heading
		: force_y_degree;
	const glm::vec3 camera_offset = camera_mode == CameraMode::Ball
		? glm::vec3(0.f, 2.f, -39.f)
		: glm::vec3(0.f, 55.f, -110.f);

	GLint viewTransform = glGetUniformLocation(shader_program, "viewTransform");
	glm::vec3 camPos(p1.x, p1.y, p1.z);
	glm::mat4 cameraTransMat = glm::mat4(1.f);
	cameraTransMat = glm::translate(cameraTransMat, glm::vec3(camPos.x, camPos.y, camPos.z));
	cameraTransMat = glm::rotate(cameraTransMat, glm::radians(camera_heading - 90.f), glm::vec3(0.f, 1.f, 0.f));
	cameraTransMat = glm::translate(cameraTransMat, camera_offset);
	camPos = cameraTransMat * glm::vec4(0.f, 0.f, 0.f, 1.f);
	glm::mat4 viewMat = glm::lookAt(camPos, p1, glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(viewTransform, 1, GL_FALSE, glm::value_ptr(viewMat));
}

bool ball_move = false;
bool force_up = false;
bool fire = false;
bool cue_striking = false;
bool aim_left_held = false;
bool aim_right_held = false;
float force_intense = 0.f;
float pending_shot_power = 0.f;
float cue_strike_offset = 0.f;
constexpr float maximum_force_gauge = 25.f;

enum class TrajectoryHitType {
	Wall,
	Ball
};

struct TrajectoryHit {
	TrajectoryHitType type = TrajectoryHitType::Wall;
	float distance = std::numeric_limits<float>::max();
	glm::vec2 normal = glm::vec2(0.f);
	Ball* ball = nullptr;
};

float distance_to_table_boundary(
	const glm::vec2& position,
	const glm::vec2& direction,
	float radius
) {
	constexpr float epsilon = 0.0001f;
	float distance = std::numeric_limits<float>::max();
	const float minimum_x = -board_width * 0.5f + radius;
	const float maximum_x = board_width * 0.5f - radius;
	const float minimum_z = -board_depth * 0.5f + radius;
	const float maximum_z = board_depth * 0.5f - radius;

	if (direction.x > epsilon)
		distance = std::min(distance, (maximum_x - position.x) / direction.x);
	else if (direction.x < -epsilon)
		distance = std::min(distance, (minimum_x - position.x) / direction.x);
	if (direction.y > epsilon)
		distance = std::min(distance, (maximum_z - position.y) / direction.y);
	else if (direction.y < -epsilon)
		distance = std::min(distance, (minimum_z - position.y) / direction.y);

	return std::max(0.f, distance);
}

TrajectoryHit find_first_trajectory_hit(
	Ball& active_ball,
	const glm::vec2& start,
	const glm::vec2& direction
) {
	constexpr float epsilon = 0.0001f;
	TrajectoryHit hit;
	const float radius = active_ball.getRad();
	const float minimum_x = -board_width * 0.5f + radius;
	const float maximum_x = board_width * 0.5f - radius;
	const float minimum_z = -board_depth * 0.5f + radius;
	const float maximum_z = board_depth * 0.5f - radius;

	auto consider_wall = [&](float distance, const glm::vec2& normal) {
		if (distance > epsilon && distance < hit.distance) {
			hit.type = TrajectoryHitType::Wall;
			hit.distance = distance;
			hit.normal = normal;
			hit.ball = nullptr;
		}
	};

	if (direction.x > epsilon)
		consider_wall((maximum_x - start.x) / direction.x, glm::vec2(-1.f, 0.f));
	else if (direction.x < -epsilon)
		consider_wall((minimum_x - start.x) / direction.x, glm::vec2(1.f, 0.f));
	if (direction.y > epsilon)
		consider_wall((maximum_z - start.y) / direction.y, glm::vec2(0.f, -1.f));
	else if (direction.y < -epsilon)
		consider_wall((minimum_z - start.y) / direction.y, glm::vec2(0.f, 1.f));

	Ball* balls[] = { &main_ball_1, &main_ball_2, &sub_ball_1, &sub_ball_2 };
	for (Ball* candidate : balls) {
		if (candidate == &active_ball)
			continue;

		const glm::vec3 candidate_position_3d = candidate->getPos();
		const glm::vec2 candidate_position(candidate_position_3d.x, candidate_position_3d.z);
		const glm::vec2 to_candidate = candidate_position - start;
		const float projection = glm::dot(to_candidate, direction);
		if (projection <= epsilon)
			continue;

		const float combined_radius = radius + candidate->getRad();
		const float perpendicular_squared =
			glm::dot(to_candidate, to_candidate) - projection * projection;
		const float radius_squared = combined_radius * combined_radius;
		if (perpendicular_squared > radius_squared)
			continue;

		const float distance = projection - std::sqrt(std::max(0.f, radius_squared - perpendicular_squared));
		if (distance <= epsilon || distance >= hit.distance)
			continue;

		const glm::vec2 collision_position = start + direction * distance;
		hit.type = TrajectoryHitType::Ball;
		hit.distance = distance;
		hit.normal = glm::normalize(candidate_position - collision_position);
		hit.ball = candidate;
	}

	return hit;
}

void draw_dotted_guide(
	Floor& dot,
	GLuint shader_program,
	const glm::vec2& start,
	const glm::vec2& direction,
	float length
) {
	constexpr float dot_spacing = 5.f;
	for (float distance = 0.f; distance <= length; distance += dot_spacing) {
		const glm::vec2 position = start + direction * distance;
		dot.set_state(glm::vec3(position.x, 0.3f, position.y), 0.f, 0.f);
		dot.Draw(shader_program);
	}
}

void draw_trajectory_guide(GLuint shader_program, Ball& active_ball, const glm::vec2& direction) {
	constexpr float maximum_bounce_guide_length = 70.f;
	const glm::vec3 ball_position_3d = active_ball.getPos();
	const glm::vec2 ball_position(ball_position_3d.x, ball_position_3d.z);
	const TrajectoryHit hit = find_first_trajectory_hit(active_ball, ball_position, direction);
	if (!std::isfinite(hit.distance))
		return;

	const float start_offset = active_ball.getRad() + 2.f;
	const glm::vec2 first_start = ball_position + direction * start_offset;
	draw_dotted_guide(
		gage_bar,
		shader_program,
		first_start,
		direction,
		std::max(0.f, hit.distance - start_offset)
	);

	const glm::vec2 collision_position = ball_position + direction * hit.distance;
	secondary_guide_dot.set_state(
		glm::vec3(collision_position.x, 0.31f, collision_position.y),
		0.f,
		0.f
	);
	secondary_guide_dot.Draw(shader_program);

	if (hit.type == TrajectoryHitType::Wall) {
		const glm::vec2 reflected = glm::normalize(
			direction - 2.f * glm::dot(direction, hit.normal) * hit.normal
		);
		const float reflected_length = std::min(
			maximum_bounce_guide_length,
			distance_to_table_boundary(collision_position, reflected, active_ball.getRad())
		);
		draw_dotted_guide(
			gage_bar,
			shader_program,
			collision_position + reflected * 2.f,
			reflected,
			reflected_length
		);
		return;
	}

	const float normal_component = glm::dot(direction, hit.normal);
	glm::vec2 cue_ball_direction = direction -
		((1.f + ball_restitution) * 0.5f * normal_component) * hit.normal;
	const float cue_direction_length = glm::length(cue_ball_direction);
	if (cue_direction_length > 0.01f) {
		cue_ball_direction /= cue_direction_length;
		const float cue_guide_length = std::min(
			maximum_bounce_guide_length,
			distance_to_table_boundary(collision_position, cue_ball_direction, active_ball.getRad())
		);
		draw_dotted_guide(
			gage_bar,
			shader_program,
			collision_position + cue_ball_direction * 2.f,
			cue_ball_direction,
			cue_guide_length
		);
	}

	if (hit.ball != nullptr) {
		const glm::vec3 hit_ball_position_3d = hit.ball->getPos();
		const glm::vec2 hit_ball_position(hit_ball_position_3d.x, hit_ball_position_3d.z);
		const float object_guide_length = std::min(
			maximum_bounce_guide_length,
			distance_to_table_boundary(hit_ball_position, hit.normal, hit.ball->getRad())
		);
		draw_dotted_guide(
			secondary_guide_dot,
			shader_program,
			hit_ball_position + hit.normal * (hit.ball->getRad() + 2.f),
			hit.normal,
			object_guide_length
		);
	}
}


void draw_world(GLuint shader_program)
{
	Ball& active_ball = player_1_turn ? main_ball_1 : main_ball_2;
	const glm::vec3 ball_position = active_ball.getPos();
	const float aim_radians = glm::radians(force_y_degree);
	const glm::vec2 aim_direction(-glm::cos(aim_radians), glm::sin(aim_radians));

	if (!ball_move) {
		draw_trajectory_guide(shader_program, active_ball, aim_direction);

		const float pull_ratio = std::clamp(force_intense / maximum_force_gauge, 0.f, 1.f);
		const float cue_offset = cue_striking
			? cue_strike_offset
			: cue_pull_distance * pull_ratio;
		const float cue_center_distance =
			active_ball.getRad() + cue_rest_gap + cue_length * 0.5f + cue_offset;
		cue_stick.set_state(
			glm::vec3(
				ball_position.x - aim_direction.x * cue_center_distance,
				ball_position.y,
				ball_position.z - aim_direction.y * cue_center_distance
			),
			force_y_degree - 90.f
		);
		cue_stick.Draw(shader_program);
	}

	billard_floor.Draw(shader_program);

	left_box.Draw(shader_program);
	top_box.Draw(shader_program);
	right_box.Draw(shader_program);
	bottom_box.Draw(shader_program);


	main_ball_1.Draw(shader_program);
	main_ball_2.Draw(shader_program);
	sub_ball_1.Draw(shader_program);
	sub_ball_2.Draw(shader_program);
};

void Key_input(unsigned char key, int x, int y) {
	switch (key)
	{
	case '1':
		view_num = 0;
		break;
	case '2':
		view_num = 1;
		break;
	case 'c':
	case 'C':
		camera_mode = camera_mode == CameraMode::Player
			? CameraMode::Ball
			: CameraMode::Player;
		glutSetWindowTitle(camera_mode == CameraMode::Player
			? "Project - Player Camera"
			: "Project - Ball Camera");
		break;
	case 27:
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
}


int red_1_touch = 0;
int red_2_touch = 0;
int oppo_touch = 0;



void special_Key(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (!ball_move && !force_up && !cue_striking) {
			aim_left_held = true;
			force_y_degree = std::fmod(force_y_degree - 0.5f + 360.f, 360.f);
		}
		break;
	case GLUT_KEY_RIGHT:
		if (!ball_move && !force_up && !cue_striking) {
			aim_right_held = true;
			force_y_degree = std::fmod(force_y_degree + 0.5f, 360.f);
		}
		break;
	case GLUT_KEY_DOWN:
		if (!ball_move && !force_up && !cue_striking) {
			force_intense = 0.f;
			cue_strike_offset = 0.f;
			force_up = true;
		}
		break;
	default:
		break;
	}

}

void special_Key_up(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		aim_left_held = false;
		return;
	}
	if (key == GLUT_KEY_RIGHT) {
		aim_right_held = false;
		return;
	}
	if (key == GLUT_KEY_DOWN && force_up && !ball_move) {
		force_up = false;
		pending_shot_power = std::clamp(
			force_intense / maximum_force_gauge,
			0.f,
			1.f
		);
		cue_strike_offset = cue_pull_distance * pending_shot_power;
		cue_striking = true;
	}
}



void resolve_all_ball_collisions() {
	const bool main_balls_collided = resolve_ball_collision(main_ball_1, main_ball_2, ball_restitution);
	const bool main_1_red_1 = resolve_ball_collision(main_ball_1, sub_ball_1, ball_restitution);
	const bool main_1_red_2 = resolve_ball_collision(main_ball_1, sub_ball_2, ball_restitution);
	const bool main_2_red_1 = resolve_ball_collision(main_ball_2, sub_ball_1, ball_restitution);
	const bool main_2_red_2 = resolve_ball_collision(main_ball_2, sub_ball_2, ball_restitution);
	resolve_ball_collision(sub_ball_1, sub_ball_2, ball_restitution);

	if (main_balls_collided)
		oppo_touch += 1;

	if (player_1_turn) {
		if (main_1_red_1)
			red_1_touch += 1;
		if (main_1_red_2)
			red_2_touch += 1;
	}
	else {
		if (main_2_red_1)
			red_1_touch += 1;
		if (main_2_red_2)
			red_2_touch += 1;
	}
}

void resolve_all_wall_collisions() {
	Ball* balls[] = { &main_ball_1, &main_ball_2, &sub_ball_1, &sub_ball_2 };
	Box* walls[] = { &left_box, &right_box, &top_box, &bottom_box };

	for (Ball* ball : balls) {
		for (Box* wall : walls)
			resolve_ball_wall_collision(*ball, *wall, cushion_restitution);
	}
}
 



bool stop_if_slow(Ball& ball) {
	constexpr float stop_speed = 0.01f;
	if (ball.getvelsize() > stop_speed)
		return false;

	ball.setVel(glm::vec2(0.f));
	return true;
}

void finish_turn() {
	const bool turn_changes = red_1_touch < 1 || red_2_touch < 1 || oppo_touch > 0;
	if (turn_changes) {
		player_1_turn = !player_1_turn;
		view_num = player_1_turn ? 0 : 1;
	}

	red_1_touch = 0;
	red_2_touch = 0;
	oppo_touch = 0;
	ball_move = false;
	camera_locked_during_shot = false;
	force_up = false;
	fire = false;
	cue_striking = false;
	aim_left_held = false;
	aim_right_held = false;
	force_intense = 0.f;
	pending_shot_power = 0.f;
	cue_strike_offset = 0.f;
}

void TimerFunc(float delta_time) {
	delta_time = std::clamp(delta_time, 0.f, 0.05f);
	if (!ball_move && !force_up && !cue_striking) {
		constexpr float aim_rotation_speed = 30.f;
		const float aim_input =
			(aim_right_held ? 1.f : 0.f) - (aim_left_held ? 1.f : 0.f);
		force_y_degree = std::fmod(
			force_y_degree + aim_input * aim_rotation_speed * delta_time + 360.f,
			360.f
		);
	}

	if (force_up) {
		constexpr float charge_speed = 16.6667f;
		force_intense = std::min(
			maximum_force_gauge,
			force_intense + charge_speed * delta_time
		);
	}
	if (cue_striking) {
		cue_strike_offset -= cue_forward_speed * delta_time;
		if (cue_strike_offset <= -cue_rest_gap) {
			cue_strike_offset = -cue_rest_gap;
			cue_striking = false;
			fire = true;
		}
	}
	if (fire) {
		PlaySoundA("billiard.wav", NULL, SND_FILENAME | SND_ASYNC);

		const glm::vec2 shot_velocity = decide_shot_velocity(
			force_y_degree, pending_shot_power, maximum_cue_impulse, ball_mass
		);
		locked_camera_focus = player_1_turn
			? main_ball_1.getPos()
			: main_ball_2.getPos();
		locked_camera_heading = force_y_degree;
		camera_locked_during_shot = true;
		if (player_1_turn) {
			main_ball_1.setVel(shot_velocity);
		}
		else {
			main_ball_2.setVel(shot_velocity);
		}
		fire = false;
		ball_move = true;
		force_intense = 0.f;
		pending_shot_power = 0.f;
		cue_strike_offset = 0.f;
	}
	if (!ball_move)
		return;

	const float maximum_speed = std::max({
		main_ball_1.getvelsize(), main_ball_2.getvelsize(),
		sub_ball_1.getvelsize(), sub_ball_2.getvelsize()
	});
	const float maximum_step_distance = ball_radius_meter * 0.5f;
	const float maximum_frame_distance = maximum_speed * delta_time;
	const int substeps = std::max(1, static_cast<int>(std::ceil(maximum_frame_distance / maximum_step_distance)));
	const float step_duration = delta_time / static_cast<float>(substeps);

	for (int step = 0; step < substeps; ++step) {
		main_ball_1.move(step_duration);
		main_ball_2.move(step_duration);
		sub_ball_1.move(step_duration);
		sub_ball_2.move(step_duration);

		constexpr int collision_solver_iterations = 4;
		for (int iteration = 0; iteration < collision_solver_iterations; ++iteration) {
			resolve_all_ball_collisions();
			resolve_all_wall_collisions();
		}
	}

	main_ball_1.setVel(apply_rolling_friction(
		main_ball_1.getVel(), main_ball_1.getMass(), coefficient_of_friction, gravity, delta_time
	));
	main_ball_2.setVel(apply_rolling_friction(
		main_ball_2.getVel(), main_ball_2.getMass(), coefficient_of_friction, gravity, delta_time
	));
	sub_ball_1.setVel(apply_rolling_friction(
		sub_ball_1.getVel(), sub_ball_1.getMass(), coefficient_of_friction, gravity, delta_time
	));
	sub_ball_2.setVel(apply_rolling_friction(
		sub_ball_2.getVel(), sub_ball_2.getMass(), coefficient_of_friction, gravity, delta_time
	));

	const bool main_1_stopped = stop_if_slow(main_ball_1);
	const bool main_2_stopped = stop_if_slow(main_ball_2);
	const bool sub_1_stopped = stop_if_slow(sub_ball_1);
	const bool sub_2_stopped = stop_if_slow(sub_ball_2);
	const bool all_stopped = main_1_stopped && main_2_stopped && sub_1_stopped && sub_2_stopped;
	if (all_stopped)
		finish_turn();
}
