#ifndef OBJ_FBZ1OUTRO_H
#define OBJ_FBZ1OUTRO_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectFBZ1Outro;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityFBZ1Outro;

// Object Struct
extern ObjectFBZ1Outro *FBZ1Outro;

// Standard Entity Events
void FBZ1Outro_Update(void);
void FBZ1Outro_LateUpdate(void);
void FBZ1Outro_StaticUpdate(void);
void FBZ1Outro_Draw(void);
void FBZ1Outro_Create(void* data);
void FBZ1Outro_StageLoad(void);
void FBZ1Outro_EditorDraw(void);
void FBZ1Outro_EditorLoad(void);
void FBZ1Outro_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_FBZ1OUTRO_H
