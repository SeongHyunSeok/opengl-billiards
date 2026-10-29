#pragma once
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

inline glm::vec2 decide_shot_velocity(
	float y_degree,
	float power_ratio,
	float maximum_cue_impulse,
	float ball_mass
) {
	const float clamped_power = std::clamp(power_ratio, 0.f, 1.f);
	const glm::vec2 direction(
		-glm::cos(glm::radians(y_degree)),
		glm::sin(glm::radians(y_degree))
	);
	if (ball_mass <= 0.f)
		return glm::vec2(0.f);

	// J = m * delta_v. Convert the resulting velocity from m/s to cm/s.
	const float impulse = maximum_cue_impulse * clamped_power;
	const float speed_centimeters_per_second = (impulse / ball_mass) * 100.f;
	return direction * speed_centimeters_per_second;
}

inline glm::vec2 apply_rolling_friction(
	glm::vec2 velocity,
	float mass,
	float coefficient_of_friction,
	float gravity,
	float delta_time
) {
	const float speed = glm::length(velocity);
	if (speed <= 0.f || mass <= 0.f || delta_time <= 0.f)
		return glm::vec2(0.f);

	// F_friction = mu * m * g, and a = F / m.
	const float friction_force = coefficient_of_friction * mass * gravity;
	const float deceleration = friction_force / mass;
	const float next_speed = std::max(0.f, speed - deceleration * delta_time);
	if (next_speed <= 0.f)
		return glm::vec2(0.f);

	return velocity * (next_speed / speed);
}

