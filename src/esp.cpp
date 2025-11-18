#include "esp.h"
#include "constants.h"
#include <stdio.h>
#include <iostream>
#include "geom.h"
#include "settings.h"
#include "imgui/imgui.h"
#include "utils/render_utils.h"
#include "utils/player_utils.h"

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
        
        teammate = !Utils::PlayerUtils::isEnemy(player);
        if (player->health > 100 || player->health <= 0 || (teammate && !Settings::ESP::drawTeam)) {
            offset += 4;
            entityIndex++;
            continue;
        }
            
        Vec3 headpos = { player->headpos.x, player->headpos.y, player->headpos.z + 0.75f };
        Vec3 feetpos = { player->pos.x, player->pos.y, player->pos.z };

        Vec3 headScreenPos = OpenGLWorldToScreen(headpos, viewMatrix, *screenWidthPtr, *screenHeightPtr);
        Vec3 feetScreenPos = OpenGLWorldToScreen(feetpos, viewMatrix, *screenWidthPtr, *screenHeightPtr);

        ImColor espColor = teammate ? *Settings::ESP::teamColor : *Settings::ESP::enemyColor;
        
        if (Settings::ESP::drawTracelines) {
            bool isValidPosition = !(feetScreenPos.x == 0 && feetScreenPos.y == 0 && feetScreenPos.z == 0);
            
            if (isValidPosition) {
                float bottomCenterX = *screenWidthPtr / 2.0f;
                float bottomCenterY = *screenHeightPtr;
                
                // clamp endpoint to screen bounds if off screen
                float endX = feetScreenPos.x;
                float endY = feetScreenPos.y;
                
                if (endX < 0) endX = 0;
                else if (endX > *screenWidthPtr) endX = *screenWidthPtr;
                if (endY < 0) endY = 0;
                else if (endY > *screenHeightPtr) endY = *screenHeightPtr;
                
                ImGui::GetBackgroundDrawList()->AddLine(
                    ImVec2(bottomCenterX, bottomCenterY),
                    ImVec2(endX, endY),
                    espColor,
                    1.0f
                );
            }
        }

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

        ImGui::GetBackgroundDrawList()->AddQuad(topLeft, topRight, bottomRight, bottomLeft, espColor, 1.0f);
        Utils::Render::drawCenteredText(player->name, centerX, feetScreenPos.y + 5);
        Utils::Render::drawHealthBar(centerX, width, headScreenPos.y, feetScreenPos.y, player->health);

        offset += 4;
        entityIndex++;
    }
}