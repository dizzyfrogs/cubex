#pragma once
#include "genCode.h"

namespace Utils {
	namespace PlayerUtils {
		bool isInFOV(Player* owner, Vec3 looking);
		bool isInFOVW2S(Vec3& screenLoc);
		bool isValidTarget(Player* target);
	}
}

