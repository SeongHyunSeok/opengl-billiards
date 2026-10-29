#pragma once
#include <algorithm>
#include <cmath>
#include "ball.h"
#include "box.h"

inline bool resolve_ball_collision(Ball& a, Ball& b, float restitution) {
	const glm::vec3 p1 = a.getPos();
	const glm::vec3 p2 = b.getPos();
	const glm::vec2 delta(p2.x - p1.x, p2.z - p1.z);
	const float minimum_distance = a.getRad() + b.getRad();
	const float distance_squared = glm::dot(delta, delta);

	if (distance_squared > minimum_distance * minimum_distance)
		return false;

	constexpr float epsilon = 0.0001f;
	const float distance = std::sqrt(std::max(distance_squared, 0.f));
	glm::vec2 normal;
	if (distance > epsilon) {
		normal = delta / distance;
	}
	else {
		const glm::vec2 relative_velocity = a.getVel() - b.getVel();
		const float relative_speed_squared = glm::dot(relative_velocity, relative_velocity);
		normal = relative_speed_squared > epsilon * epsilon
			? relative_velocity / std::sqrt(relative_speed_squared)
			: glm::vec2(1.f, 0.f);
	}

	const float penetration = minimum_distance - distance;
	const float inverse_mass_a = a.getMass() > 0.f ? 1.f / a.getMass() : 0.f;
	const float inverse_mass_b = b.getMass() > 0.f ? 1.f / b.getMass() : 0.f;
	const float inverse_mass_sum = inverse_mass_a + inverse_mass_b;
	if (inverse_mass_sum <= 0.f)
		return false;

	if (penetration > 0.f) {
		const glm::vec2 correction = normal * ((penetration + epsilon) / inverse_mass_sum);
		a.position.x -= correction.x * inverse_mass_a;
		a.position.z -= correction.y * inverse_mass_a;
		b.position.x += correction.x * inverse_mass_b;
		b.position.z += correction.y * inverse_mass_b;
	}

	const float approaching_speed = glm::dot(a.getVel() - b.getVel(), normal);
	if (approaching_speed <= 0.f)
		return false;

	// Equal and opposite impulses: J = -(1 + e) * relative_normal_speed / sum(1 / mass).
	const float clamped_restitution = std::clamp(restitution, 0.f, 1.f);
	const float impulse_magnitude =
		(1.f + clamped_restitution) * approaching_speed / inverse_mass_sum;
	const glm::vec2 impulse = impulse_magnitude * normal;
	a.setVel(a.getVel() - impulse * inverse_mass_a);
	b.setVel(b.getVel() + impulse * inverse_mass_b);
	return true;
}

inline bool resolve_ball_wall_collision(Ball& ball, Box& box, float restitution) {
	const glm::vec3 position = ball.getPos();
	const glm::vec3 box_position = box.getPos();
	const float radius = ball.getRad();
	const float half_wall_width = box.getW() * 0.5f;
	const int rotation = static_cast<int>(box.getY_Deg());
	glm::vec2 inward_normal(0.f);

	if (rotation == 0) {
		const float inner_face = box_position.x - half_wall_width;
		if (position.x + radius < inner_face || ball.getVel().x <= 0.f)
			return false;
		ball.position.x = inner_face - radius;
		inward_normal = glm::vec2(-1.f, 0.f);
	}
	else if (rotation == 180) {
		const float inner_face = box_position.x + half_wall_width;
		if (position.x - radius > inner_face || ball.getVel().x >= 0.f)
			return false;
		ball.position.x = inner_face + radius;
		inward_normal = glm::vec2(1.f, 0.f);
	}
	else if (rotation == 90) {
		const float inner_face = box_position.z - half_wall_width;
		if (position.z + radius < inner_face || ball.getVel().y <= 0.f)
			return false;
		ball.position.z = inner_face - radius;
		inward_normal = glm::vec2(0.f, -1.f);
	}
	else if (rotation == -90) {
		const float inner_face = box_position.z + half_wall_width;
		if (position.z - radius > inner_face || ball.getVel().y >= 0.f)
			return false;
		ball.position.z = inner_face + radius;
		inward_normal = glm::vec2(0.f, 1.f);
	}
	else {
		return false;
	}

	// The wall has effectively infinite mass, so only the ball receives the impulse.
	const float normal_speed = glm::dot(ball.getVel(), inward_normal);
	const float clamped_restitution = std::clamp(restitution, 0.f, 1.f);
	ball.setVel(ball.getVel() - (1.f + clamped_restitution) * normal_speed * inward_normal);
	return true;
}
