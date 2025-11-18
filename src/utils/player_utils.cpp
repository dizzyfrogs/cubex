#include "player_utils.h"
#include "constants.h"
#include "geom.h"
#include "settings.h"
#include "utils/math_utils.h"
#include <cmath>

bool Utils::PlayerUtils::isInFOV(Player* owner, Vec3 looking) {
	Vec3 angle = CalcAngle(owner->headpos, looking);
	angle.x += 90.0f;
	Utils::Math::normalizeAngle(angle);

	Vec3 playerAngle(owner->yaw, owner->pitch, 0);
	Utils::Math::normalizeAngle(playerAngle);

	float yawDiff = Utils::Math::getShortestAngleDiff(playerAngle.x, angle.x);
	float pitchDiff = fabs(playerAngle.y - angle.y);

	return (yawDiff < fov / 2.0f && pitchDiff < fov / 2.0f);
}

bool Utils::PlayerUtils::isInFOVW2S(Vec3& screenLoc) {
	return abs(Utils::Math::getCenterScreenPos().Distance(screenLoc)) < Settings::Aimbot::fov;
}

bool Utils::PlayerUtils::isValidTarget(Player* target) {
	return target && localPlayerPtr && target->health <= 100 && target->health > 0 && Utils::PlayerUtils::isInFOV(localPlayerPtr, target->headpos);
}

bool Utils::PlayerUtils::isTeamGameMode(int mode) {
	// team game modes: 0, 4, 5, 7, 11, 13, 14, 16, 17
	return mode == 0 || mode == 4 || mode == 5 || mode == 7 || mode == 11 || 
	       mode == 13 || mode == 14 || mode == 16 || mode == 17;
}

bool Utils::PlayerUtils::isEnemy(Player* player) {
	if (!player || !localPlayerPtr)
		return false;
	
	if (!isTeamGameMode(gameMode)) {
		return true;
	}
	
	return player->team != localPlayerPtr->team;
}

