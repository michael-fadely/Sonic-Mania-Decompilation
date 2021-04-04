#include "../SonicMania.h"

ObjectBSS_Collected *BSS_Collected;

void BSS_Collected_Update(void)
{
    RSDK_THIS(BSS_Collected);
    EntityBSS_Setup *setup = (EntityBSS_Setup *)RSDK.GetEntityByID(SLOT_BSS_SETUP);
    int fieldPos           = entity->position.y + (BSS_PLAYFIELD_W * entity->position.x);
    switch (entity->type) {
        case 0:
            ++BSS_Setup->field_1C;
            BSS_Setup->field_1C &= 0xF;
            if (++entity->timer >= 16 && setup->state == BSS_Setup_State_HandleStage) {
                BSS_Setup->playField[fieldPos] = BSS_NONE;
                RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
            }
            break;
        case 1:
            if (BSS_Setup->sphereCount <= 0) {
                if (BSS_Setup->playField[fieldPos] != 130) {
                    RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                }
                else {
                    BSS_Setup->playField[fieldPos] = 2;
                    RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                }
            }
            else {
                if (setup->globeTimer < 32 || setup->globeTimer > 224)
                    entity->type = 2;
            }
            break;
        case 2:
            if (setup->state == BSS_Setup_State_HandleStage) {
                if (setup->globeTimer > 32 && setup->globeTimer < 224) {
                    if (BSS_Setup->playField[fieldPos] != BSS_BLUE_STOOD) {
                        RSDK.ResetEntityPtr(entity, 0, 0);
                    }
                    else {
                        BSS_Setup->playField[fieldPos] = BSS_SPHERE_RED;
                        RSDK.ResetEntityPtr(entity, 0, 0);
                    }
                }
            }
            break;
        case 3:
            if (setup->globeTimer < 32 || setup->globeTimer > 224) {
                entity->timer = 10;
                entity->type  = 4;
            }
            break;
        case 4:
            if (setup->state == BSS_Setup_State_HandleStage && --entity->timer <= 0) {
                if (BSS_Setup->playField[fieldPos] != BSS_SPHERE_GREEN_STOOD) {
                    RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                }
                else {
                    BSS_Setup->playField[fieldPos] = BSS_SPHERE_BLUE;
                    RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                }
            }
            break;
        case 5:
            if (setup->state == BSS_Setup_State_HandleStage) {
                if (setup->playerPos.x != entity->position.x || setup->playerPos.y != entity->position.y) {
                    if (BSS_Setup->playField[fieldPos] == BSS_SPHERE_PINK_STOOD) {
                        BSS_Setup->playField[fieldPos] = BSS_SPHERE_PINK;
                        RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                    }
                    else {
                        RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                    }
                }
            }
            break;
        default: break;
    }
}

void BSS_Collected_LateUpdate(void) {}

void BSS_Collected_StaticUpdate(void) {}

void BSS_Collected_Draw(void) {}

void BSS_Collected_Create(void *data)
{
    RSDK_THIS(BSS_Collected);
    if (!RSDK_sceneInfo->inEditor) {
        entity->active = ACTIVE_NORMAL;
        entity->type   = voidToInt(data);
    }
}

void BSS_Collected_StageLoad(void) {}

void BSS_Collected_EditorDraw(void) {}

void BSS_Collected_EditorLoad(void) {}

void BSS_Collected_Serialize(void) {}
