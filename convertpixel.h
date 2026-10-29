#pragma once
#define _CRT_SECURE_NO_WARNINGS

float PIXEL_PER_METER = (10.0 / 0.3);  // 10 pixel 30 cm

float convert_to_pixel(float meter) {
	return meter * PIXEL_PER_METER;
}