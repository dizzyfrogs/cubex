#include "aimbot.h"
#include "constants.h"
#include <stdio.h>
#include <iostream>
#include "geom.h"
#include "settings.h"
#include "imgui/imgui.h"
#include <ctime>
#include <random>
#include "libs/perlin.h"
#include "utils/math_utils.h"
#include "utils/player_utils.h"

float curAimTime = 0;
clock_t lastAimTime = clock();
Player* curTarget = nullptr;
static siv::PerlinNoise perlinNoise(std::mt19937{ std::random_device{}() });
static float noiseTime = 0.0f;

Player* Aimbot::getNearestEntityW2S() {
    if (!localPlayerPtr || !entityListBase || numPlayers <= 0)
        return nullptr;
        
    Player* nearestPlayer = nullptr;
    float nearestDistance = 9999999.0f;
    float distance = 0;
    uintptr_t listBasePtr = *(uintptr_t*)entityListBase;
    
    if (!listBasePtr)
        return nullptr;

    int offset = 4;
    int entityIndex = 1;

    while (entityIndex < numPlayers) {
        Player* player = *(Player**)(listBasePtr + offset);

        if (!player || player->health <= 0 || player->health > 100 || player->team == localPlayerPtr->team) {
            offset += 4;
            entityIndex++;
            continue;
        }

        Vec3 headpos = player->headpos;
        Vec3 headScreenPos = OpenGLWorldToScreen(headpos, viewMatrix, *screenWidthPtr, *screenHeightPtr);

        if (headScreenPos.z < 0 || (Settings::Aimbot::checkInFov && !Utils::PlayerUtils::isInFOVW2S(headScreenPos))) {
            offset += 4;
            entityIndex++;
            continue;
        }

        headScreenPos.z = 0;
        distance = abs(Utils::Math::getCenterScreenPos().Distance(headScreenPos));

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestPlayer = player;
        }

        offset += 4;
        entityIndex++;
    }
    return nearestPlayer;
}

void Aimbot::aimbot() {
    if (!localPlayerPtr)
        return;
        
    if (Settings::Aimbot::drawFovCircle) {
        Vec3 centerPos = Utils::Math::getCenterScreenPos();
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(centerPos.x, centerPos.y), Settings::Aimbot::fov, IM_COL32(255, 255, 255, 255 / 2), 50);
    }
    if (!Settings::Aimbot::enabled || !GetAsyncKeyState(Settings::Aimbot::aimKey)) {
        curAimTime = 0;
        lastAimTime = clock();
        curTarget = nullptr;
        return;
    }
    Player* target = getNearestEntityW2S();
    if (!target)
        return;

    if (target != curTarget) {
        curAimTime = 0;
        lastAimTime = clock();
        curTarget = target;
    }

    clock_t now = clock();
    curAimTime += static_cast<float>(now - lastAimTime) / CLOCKS_PER_SEC;
    lastAimTime = now;
    float percent = curAimTime / Settings::Aimbot::smoothingAmount;
    if (percent > 1.0f) {
        percent = 1.0f;
    }

    Vec3 targetAngle = CalcAngle(localPlayerPtr->headpos, target->headpos);
    targetAngle.x += 90;

    Vec3 currentAngle = { localPlayerPtr->yaw, localPlayerPtr->pitch, 0 };
    
    if (Settings::Aimbot::smoothing) {
        if (percent >= 1) {
            curAimTime = 0;
            percent = 1;
        }
        Utils::Math::smoothAngle(currentAngle, targetAngle, percent);
    }
    else {
        currentAngle = targetAngle;
    }
    
    if (Settings::Aimbot::randomizationEnabled) {
        static clock_t lastNoiseTime = clock();
        float deltaTime = static_cast<float>(now - lastNoiseTime) / CLOCKS_PER_SEC;
        lastNoiseTime = now;
        noiseTime += deltaTime * Settings::Aimbot::randomizationSpeed;

        double noiseYaw = perlinNoise.noise1D(noiseTime);
        double noisePitch = perlinNoise.noise1D(noiseTime + 1000.0); // offset by 1000 to get different noise
        
        currentAngle.x += static_cast<float>(noiseYaw) * Settings::Aimbot::randomizationIntensityYaw;
        currentAngle.y += static_cast<float>(noisePitch) * Settings::Aimbot::randomizationIntensityPitch;
        
        Utils::Math::normalizeAngle(currentAngle);
    }
    
    localPlayerPtr->yaw = currentAngle.x;
    localPlayerPtr->pitch = currentAngle.y;
}

