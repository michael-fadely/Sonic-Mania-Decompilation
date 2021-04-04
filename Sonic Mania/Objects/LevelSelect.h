#ifndef OBJ_LEVELSELECT_H
#define OBJ_LEVELSELECT_H

#include "../SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    int value1; //= 240;
    int value2[4]; //= { 240, 3, 3, 3 };
    int value3[9]; //= { 1, 9, 7, 9, 0, 8, 1, 1, 255 };
    int value4[5]; //= { 4, 1, 2, 6, 255 };
    int value5[9]; //= { 1, 9, 9, 2, 1, 1, 2, 4, 255 };
    int value6[9]; //= { 2, 0, 1, 8, 0, 6, 2, 3, 255 };
    int value7[9]; //= { 1, 9, 8, 9, 0, 5, 0, 1, 255 };
    int value8[9]; //= { 2, 0, 1, 7, 0, 8, 1, 5, 255 };
    int value9[5]; //= { 9, 0, 0, 1, 255 };
    int value10[5]; //= { 6, 2, 1, 4, 255 };
    int value11;
    int value12;
    int value13;
    ushort value14;
    ushort value15;
    ushort value16;
    ushort value17;
    ushort value18;
    bool32 value19;
    int value20;
    byte value21;
} ObjectLevelSelect;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityLevelSelect;

// Object Struct
extern ObjectLevelSelect *LevelSelect;

// Standard Entity Events
void LevelSelect_Update(void);
void LevelSelect_LateUpdate(void);
void LevelSelect_StaticUpdate(void);
void LevelSelect_Draw(void);
void LevelSelect_Create(void* data);
void LevelSelect_StageLoad(void);
void LevelSelect_EditorDraw(void);
void LevelSelect_EditorLoad(void);
void LevelSelect_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_LEVELSELECT_H
