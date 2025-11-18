#include "math_utils.h"
#include "constants.h"
#include <cmath>

Vec3 Utils::Math::getCenterScreenPos() {
	Vec3 centerScreenPos = { (float)*screenWidthPtr / 2.0f, (float)*screenHeightPtr / 2.0f, 0.0f };
	return centerScreenPos;
}

void Utils::Math::normalizeAngle(Vec3& angle) {
	while (angle.x >= 360.0f)
		angle.x -= 360.0f;
	while (angle.x < 0.0f)
		angle.x += 360.0f;

	if (angle.y > 90.0f)
		angle.y = 90.0f;
	if (angle.y < -90.0f)
		angle.y = -90.0f;
}

float Utils::Math::getShortestAngleDiff(float a1, float a2) {
	float diff = a1 - a2;
	while (diff > 180.0f)
		diff -= 360.0f;
	while (diff < -180.0f)
		diff += 360.0f;
	return fabs(diff);
}

void Utils::Math::smoothAngle(Vec3& from, Vec3& to, float percent) {
	normalizeAngle(from);
	normalizeAngle(to);

	float yawDiff = to.x - from.x;
	if (yawDiff > 180.0f) yawDiff -= 360.0f;
	if (yawDiff < -180.0f) yawDiff += 360.0f;

	float pitchDiff = to.y - from.y;

	from.x += yawDiff * percent;
	from.y += pitchDiff * percent;
	normalizeAngle(from);
}

