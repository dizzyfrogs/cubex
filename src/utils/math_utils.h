#pragma once
#include "geom.h"

namespace Utils {
	namespace Math {
		Vec3 getCenterScreenPos();
		void normalizeAngle(Vec3& angle);
		float getShortestAngleDiff(float a1, float a2);
		void smoothAngle(Vec3& from, Vec3& to, float percent);
	}
}

