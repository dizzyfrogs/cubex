#include "esp.h"
#include "constants.h"
#include <stdio.h>
#include <iostream>
#include "geom.h"
#include "settings.h"
#include "imgui/imgui.h"
#include <ctime>
#include <random>
#include "libs/perlin.h"

float curAimTime = 0;
clock_t lastAimTime = clock();
Player* curTarget = nullptr;
static siv::PerlinNoise perlinNoise(std::mt19937{ std::random_device{}() });
static float noiseTime = 0.0f;

#define min(a, b) ((a) < (b) ? (a) : (b))

Vec3 getCenterScreenPos() {
    Vec3 centerScreenPos = { (float)*screenWidthPtr / 2.0f, (float)*screenHeightPtr / 2.0f, 0.0f };
    return centerScreenPos;
}

void normalizeAngle(Vec3& angle) {
    while (angle.x >= 360.0f)
        angle.x -= 360.0f;
    while (angle.x < 0.0f)
        angle.x += 360.0f;

    if (angle.y > 90.0f)
        angle.y = 90.0f;
    if (angle.y < -90.0f)
        angle.y = -90.0f;
}

float getShortestAngleDiff(float a1, float a2) {
    float diff = a1 - a2;
    while (diff > 180.0f)
        diff -= 360.0f;
    while (diff < -180.0f)
        diff += 360.0f;
    return fabs(diff);
}

bool isInFOV(Player* owner, Vec3 looking) {
    Vec3 angle = CalcAngle(owner->headpos, looking);
    angle.x += 90.0f;
    normalizeAngle(angle);

    Vec3 playerAngle(owner->yaw, owner->pitch, 0);
    normalizeAngle(playerAngle);

    float yawDiff = getShortestAngleDiff(playerAngle.x, angle.x);
    float pitchDiff = fabs(playerAngle.y - angle.y);

    return (yawDiff < fov / 2.0f && pitchDiff < fov / 2.0f);
}

bool isInFOVW2S(Vec3& screenLoc) {
    return abs(getCenterScreenPos().Distance(screenLoc)) < Settings::Aimbot::fov;
}

bool isValidTarget(Player* target) {
    return target && localPlayerPtr && target->health <= 100 && target->health > 0 && isInFOV(localPlayerPtr, target->headpos);
}

void smoothAngle(Vec3& from, Vec3& to, float percent) {
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

Player* ESP::getNearestEntityW2S() {
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

        if (headScreenPos.z < 0 || (Settings::Aimbot::checkInFov && !isInFOVW2S(headScreenPos))) {
            offset += 4;
            entityIndex++;
            continue;
        }

        headScreenPos.z = 0;
        distance = abs(getCenterScreenPos().Distance(headScreenPos));

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestPlayer = player;
        }

        offset += 4;
        entityIndex++;
    }
    return nearestPlayer;
}

void ESP::aimbot() {
    if (!localPlayerPtr)
        return;
        
    if (Settings::Aimbot::drawFovCircle)
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(getCenterScreenPos().x, getCenterScreenPos().y), Settings::Aimbot::fov, IM_COL32(255, 255, 255, 255 / 2), 50);
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
    float percent = min(curAimTime / Settings::Aimbot::smoothingAmount, 1.0);

    Vec3 targetAngle = CalcAngle(localPlayerPtr->headpos, target->headpos);
    targetAngle.x += 90;

    Vec3 currentAngle = { localPlayerPtr->yaw, localPlayerPtr->pitch, 0 };
    
    if (Settings::Aimbot::smoothing) {
        if (percent >= 1) {
            curAimTime = 0;
            percent = 1;
        }
        smoothAngle(currentAngle, targetAngle, percent);
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
        
        normalizeAngle(currentAngle);
    }
    
    localPlayerPtr->yaw = currentAngle.x;
    localPlayerPtr->pitch = currentAngle.y;
}

void drawCenteredText(std::string text, float x, float y) {
    float textWidth = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(x - textWidth / 2, y), IM_COL32(255, 255, 255, 255), text.c_str());
}

ImColor getHealthColor(float health, float maxHealth) {
    float healthPercent = health / maxHealth;
    
    if (healthPercent > 0.5f) {
        float t = (healthPercent - 0.5f) / 0.5f;
        int r = (int)(255 * (1.0f - t));
        int g = 255;
        int b = 0;
        return ImColor(r, g, b, 255);
    } else {
        float t = healthPercent / 0.5f;
        int r = 255;
        int g = (int)(255 * t);
        int b = 0;
        return ImColor(r, g, b, 255);
    }
}

void drawHealthBar(float centerX, float boxWidth, float headY, float feetY, float health) {
    float barWidth = 3.0f;
    float barOffset = 5.0f;
    float barLeftX = centerX - boxWidth - barOffset - barWidth;
    float barRightX = centerX - boxWidth - barOffset;
    
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(barLeftX, headY), ImVec2(barRightX, feetY), IM_COL32(1, 1, 1, 255), 0, 0, 1.0f);
    
    float healthPercent = health / 100.0f;
    float filledHeight = (feetY - headY) * healthPercent;
    float filledTopY = feetY - filledHeight;
    
    ImColor healthColor = getHealthColor(health, 100.0f);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(barLeftX, filledTopY), ImVec2(barRightX, feetY), healthColor);
}

void ESP::drawESP() {
    if (!Settings::ESP::enabled)
        return;

    if (!localPlayerPtr || !entityListBase || numPlayers <= 0)
        return;

    bool teammate = false;
    uintptr_t listBasePtr = *(uintptr_t*)entityListBase;
    
    if (!listBasePtr)
        return;

    int offset = 4;
    int entityIndex = 1;

    while (entityIndex < numPlayers) {
        Player* player = *(Player**)(listBasePtr + offset);
        
        if (!player) {
            offset += 4;
            entityIndex++;
            continue;
        }
        
        teammate = player->team == localPlayerPtr->team;
        if (player->health > 100 || player->health <= 0 || (teammate && !Settings::ESP::drawTeam)) {
            offset += 4;
            entityIndex++;
            continue;
        }
            
        Vec3 headpos = { player->headpos.x, player->headpos.y, player->headpos.z + 0.75f };
        Vec3 feetpos = { player->pos.x, player->pos.y, player->pos.z };

        Vec3 headScreenPos = OpenGLWorldToScreen(headpos, viewMatrix, *screenWidthPtr, *screenHeightPtr);
        Vec3 feetScreenPos = OpenGLWorldToScreen(feetpos, viewMatrix, *screenWidthPtr, *screenHeightPtr);

        bool headOutOfBounds = headScreenPos.x < -100 || headScreenPos.x > *screenWidthPtr + 100 || headScreenPos.y < -100 || headScreenPos.y > *screenHeightPtr + 100 ||
                               (headScreenPos.x == 0 && headScreenPos.y == 0 && headScreenPos.z == 0);
        bool feetOutOfBounds = feetScreenPos.x < -100 || feetScreenPos.x > *screenWidthPtr + 100 || feetScreenPos.y < -100 || feetScreenPos.y > *screenHeightPtr + 100 ||
                               (feetScreenPos.x == 0 && feetScreenPos.y == 0 && feetScreenPos.z == 0);

        if (headOutOfBounds && feetOutOfBounds) {
            offset += 4;
            entityIndex++;
            continue;
        }

        float height = abs(headScreenPos.y - feetScreenPos.y);
        float width = height / 4;
        float centerX = (headScreenPos.x + feetScreenPos.x) / 2.0f;

        ImVec2 topLeft = ImVec2(centerX - width, headScreenPos.y);
        ImVec2 topRight = ImVec2(centerX + width, headScreenPos.y);
        ImVec2 bottomLeft = ImVec2(centerX - width, feetScreenPos.y);
        ImVec2 bottomRight = ImVec2(centerX + width, feetScreenPos.y);

        ImColor espColor = teammate ? *Settings::ESP::teamColor : *Settings::ESP::enemyColor;
        ImGui::GetBackgroundDrawList()->AddQuad(topLeft, topRight, bottomRight, bottomLeft, espColor, 1.0f);
        drawCenteredText(player->name, centerX, feetScreenPos.y + 5);
        drawHealthBar(centerX, width, headScreenPos.y, feetScreenPos.y, player->health);

        offset += 4;
        entityIndex++;
    }
}