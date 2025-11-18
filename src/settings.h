#pragma once

class ImColor;

namespace Settings {
	namespace ESP {
		extern bool enabled;
		extern bool drawTeam;
		extern ImColor* teamColor;
		extern ImColor* enemyColor;
	}
	namespace Aimbot {
		extern bool enabled;
		extern bool smoothing;
		extern float smoothingAmount;
		extern bool checkInFov;
		extern float fov;
		extern bool drawFovCircle;
		extern int aimKey;
		extern bool waitingForKeyBind;
		extern bool randomizationEnabled;
		extern float randomizationIntensityYaw;
		extern float randomizationIntensityPitch;
		extern float randomizationSpeed;
	}
}
