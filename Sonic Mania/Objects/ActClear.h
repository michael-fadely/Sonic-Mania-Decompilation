#ifndef OBJ_ACTCLEAR_H
#define OBJ_ACTCLEAR_H

#include "../SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    ushort spriteIndex;
    ushort sfx_ScoreAdd;
    ushort sfx_ScoreTotal;
    ushort sfx_Event;
    bool32 field_C;
    int field_10;
    int field_14;
    int actID;
    int field_1C;
    int field_20;
    void (*bufferMove_CB)(void);
    void (*saveReplay_CB)(void);
    int field_2C;
    int field_30;
    bool32 dword34;
} ObjectActClear;

// Entity Class
typedef struct {
    RSDK_ENTITY
    void (*state)(void);
    int timer;
    int stageFinishTimer;
    int scoreBonus;
    int field_68;
    int dword6C;
    int field_70;
    int time;
    int dword78;
    int field_7C;
    int field_80;
    int field_84;
    Vector2 posUnknown;
    Vector2 posUnknown3;
    Vector2 posUnknown2;
    Vector2 posUnknown4;
    Vector2 posUnknown5;
    Vector2 posUnknown6;
    void *playerPtr;
    AnimationData data1;
    AnimationData data2;
    AnimationData playerNameData;
    AnimationData gotThroughData;
    AnimationData actNoData;
    AnimationData data3;
} EntityActClear;

// Object Struct
extern ObjectActClear *ActClear;

// Standard Entity Events
void ActClear_Update(void);
void ActClear_LateUpdate(void);
void ActClear_StaticUpdate(void);
void ActClear_Draw(void);
void ActClear_Create(void* data);
void ActClear_StageLoad(void);
void ActClear_EditorDraw(void);
void ActClear_EditorLoad(void);
void ActClear_Serialize(void);

// Extra Entity Functions
void ActClear_DrawTime(int mins, Vector2 *pos, int secs, int millisecs);
void ActClear_DrawNumbers(Vector2 *pos, int value, signed int maxVals);
void ActClear_CheckPlayerVictory(void);
void ActClear_SaveGameCallback(int success);
void ActClear_Unknown5(void);

void ActClear_Unknown6(void);
void ActClear_Unknown7(void);
void ActClear_State_TAFinish(void);
void ActClear_Unknown8(void);
void ActClear_TallyScore(void);
void ActClear_LoadNextScene(void);
void ActClear_Unknown9(void);
void ActClear_Unknown10(void);
void ActClear_State_ActFinish(void);

void ActClear_ForcePlayerOnScreen(void);
StatInfo *ActClear_TrackActClear(byte act, byte zone, StatInfo *stat, byte charID, int time, int rings, int score);
void ActClear_GetTimeFromValue(int time, int *minsPtr, int *secsPtr, int *millisecsPtr);

#endif //!OBJ_ACTCLEAR_H
