#ifndef OBJ_BREAKBAR_H
#define OBJ_BREAKBAR_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectBreakBar;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityBreakBar;

// Object Struct
extern ObjectBreakBar *BreakBar;

// Standard Entity Events
void BreakBar_Update(void);
void BreakBar_LateUpdate(void);
void BreakBar_StaticUpdate(void);
void BreakBar_Draw(void);
void BreakBar_Create(void* data);
void BreakBar_StageLoad(void);
void BreakBar_EditorDraw(void);
void BreakBar_EditorLoad(void);
void BreakBar_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_BREAKBAR_H
