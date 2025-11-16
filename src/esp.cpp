#include "esp.h"
#include "constants.h"
#include <stdio.h>
#include <iostream>
#include "geom.h"

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

bool isValidTarget(Player* target) {
    return target && target->health <= 100 && target->health > 0 && isInFOV(localPlayerPtr, target->headpos);
}

Player* ESP::getNearestPlayer() {
	Player* nearestPlayer = nullptr;
	float nearestDistance = 9999999.0f;
    uintptr_t listBasePtr = *(uintptr_t*)entityListBase;

    int offset = 4;
    int entityIndex = 1;

    while (entityIndex < numPlayers) {
        Player* player = *(Player**)(listBasePtr + offset);

        if (!isValidTarget(player)) {
            offset += 4;
            entityIndex++;
            continue;
        }

        float distance = localPlayerPtr->pos.Distance(player->pos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestPlayer = player;
        }

        offset += 4;
        entityIndex++;
    }
    return nearestPlayer;
}

Player* ESP::getNearestEntityAngle() {
    Vec3 playerAngle(localPlayerPtr->yaw + 180, localPlayerPtr->pitch, 0);
    normalizeAngle(playerAngle);
    Player* nearestPlayer = nullptr;
    float smallestAngle = 9999999.0f;
    uintptr_t listBasePtr = *(uintptr_t*)entityListBase;

    int offset = 4;
    int entityIndex = 1;

    while (entityIndex < numPlayers) {
        Player* player = *(Player**)(listBasePtr + offset);

        if (!isValidTarget(player)) {
            offset += 4;
            entityIndex++;
            continue;
        }

        Vec3 targetAngle = CalcAngle(localPlayerPtr->pos, player->pos);
        Vec3 angleDiff = playerAngle - targetAngle;
        normalizeAngle(angleDiff);
        float angleMagnitude = angleDiff.Length();

        if (angleMagnitude < smallestAngle) {
            smallestAngle = angleMagnitude;
            nearestPlayer = player;
        }

        offset += 4;
        entityIndex++;
    }
    return nearestPlayer;
}

void ESP::aimbot() {
    if (!GetAsyncKeyState(VK_SHIFT))
        return;
    Player* target = getNearestEntityAngle();
    if (!target)
        return;

    Vec3 angle = CalcAngle(localPlayerPtr->headpos, target->headpos);
    angle.x += 90;
    localPlayerPtr->yaw = angle.x;
    localPlayerPtr->pitch = angle.y;
}