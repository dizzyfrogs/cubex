#pragma once
#include "genCode.h"

namespace Utils {
	namespace PlayerUtils {
		bool isInFOV(Player* owner, Vec3 looking);
		bool isInFOVW2S(Vec3& screenLoc);
		bool isValidTarget(Player* target);
		bool isTeamGameMode(int mode);
		bool isEnemy(Player* player);
		void traceLine(Vec3* from, Vec3* to);
		bool isVisible(Player* target);
	}
}

