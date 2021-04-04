#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include "../SonicMania.h"

enum PLAYER_ANI {
    ANI_IDLE           = 0x0,
    ANI_BORED1         = 0x1,
    ANI_BORED2         = 0x2,
    ANI_LOOKUP         = 0x3,
    ANI_CROUCH         = 0x4,
    ANI_WALK           = 0x5,
    ANI_AIRWALK        = 0x6,
    ANI_JOG            = 0x7,
    ANI_RUN            = 0x8,
    ANI_DASH           = 0x9,
    ANI_JUMP           = 0xA,
    ANI_SPRINGTWIRL    = 0xB,
    ANI_SPRINGDIAGONAL = 0xC,
    ANI_SKID           = 0xD,
    ANI_SKIDTURN       = 0xE,
    ANI_SPINDASH       = 0xF,
    ANI_DROPDASH       = 0x10,
    ANI_PUSH           = 0x11,
    ANI_HURT           = 0x12,
    ANI_DIE            = 0x13,
    ANI_DROWN          = 0x14,
    ANI_BALANCE1       = 0x15,
    ANI_BALANCE2       = 0x16,
    ANI_SPRINGCS       = 0x17,
    ANI_STANDCS        = 0x18,
    ANI_FAN            = 0x19,
    ANI_VICTORY        = 0x1A,
    ANI_OUTTAHERE      = 0x1B,
    ANI_HANG           = 0x1C,
    ANI_HANGMOVE       = 0x1D,
    ANI_POLESWINGH     = 0x1E,
    ANI_POLESWINGV     = 0x1F,
    ANI_SHAFTSWING     = 0x20,
    ANI_TURNTABLE      = 0x21,
    ANI_TWISTER        = 0x22,
    ANI_SPIRALRUN      = 0x23,
    ANI_STICK          = 0x24,
    ANI_PULLEYHOLD     = 0x25,
    ANI_SHIMMYIDLE     = 0x26,
    ANI_SHIMMYMOVE     = 0x27,
    ANI_BREATHE        = 0x28,
    ANI_BUBBLE         = 0x29,
    ANI_RIDE           = 0x2A,
    ANI_CLING          = 0x2B,
    ANI_BUNGEE         = 0x2C,
    ANI_TWISTRUN       = 0x2D,
    ANI_FLUME          = 0x2E,
    ANI_TRANSFORM      = 0x2F,
    ANI_FLY            = 0x30,
    ANI_FLYTIRED       = 0x31,
    ANI_FLYLIFT        = 0x32,
    ANI_FLYLIFTTIRED   = 0x33,
    ANI_SWIM           = 0x34,
    ANI_SWIMTIRED      = 0x35,
    ANI_SWIMLIFT       = 0x36,
};

enum ShieldTypes {
    SHIELD_NONE      = 0,
    SHIELD_BLUE      = 1,
    SHIELD_BUBBLE    = 2,
    SHIELD_FIRE      = 3,
    SHIELD_LIGHTNING = 4,
};

// Object Class
#if RETRO_USE_PLUS
typedef struct {
    RSDK_OBJECT
    int sonicPhysicsTable[0x40];
    int tailsPhysicsTable[0x40];
    int knuxPhysicsTable[0x40];
    int mightyPhysicsTable[0x40];
    int rayPhysicsTable[0x40];
    colour superPalette_Sonic[18];
    colour superPalette_Sonic_CPZ[18];
    colour superPalette_Sonic_HCZ[18];
    colour superPalette_Tails[18];
    colour superPalette_Tails_CPZ[18];
    colour superPalette_Tails_HCZ[18];
    colour superPalette_Knux[18];
    colour superPalette_Knux_CPZ[18];
    colour superPalette_Knux_HCZ[18];
    colour superPalette_Mighty[18];
    colour superPalette_Mighty_CPZ[18];
    colour superPalette_Mighty_HCZ[18];
    colour superPalette_Ray[18];
    colour superPalette_Ray_CPZ[18];
    colour superPalette_Ray_HCZ[18];
    int field_93C;
    int playerCount;
    ushort upState;
    ushort downState;
    ushort leftState;
    ushort rightState;
    ushort jumpPressState;
    ushort jumpHoldState;
    int flyCarryPosA;
    int flyCarryPosB;
    Vector2 flyCarryPositions[0x10];
    Vector2 curFlyCarryPos;
    int p2UnknownInputTimer;
    int jumpInDelay;
    int p2InputDelay;
    int field_9EC;
    int rings;
    int ringExtraLife;
    int powerups;
    int savedLives;
    int savedScore;
    int savedScore1UP;
    ushort sonicSpriteIndex;
    ushort superSpriteIndex;
    ushort tailsSpriteIndex;
    ushort tailsTailsSpriteIndex;
    ushort knuxSpriteIndex;
    ushort mightySpriteIndex;
    ushort raySpriteIndex;
    ushort sfx_Jump;
    ushort sfx_Roll;
    ushort sfx_Charge;
    ushort sfx_Release;
    ushort sfx_PeelCharge;
    ushort sfx_PeelRelease;
    ushort sfx_Dropdash;
    ushort sfx_LoseRings;
    ushort sfx_Hurt;
    ushort sfx_PimPom;
    ushort sfx_Skidding;
    ushort sfx_Grab;
    ushort sfx_Flying;
    bool32 playingFlySFX;
    ushort sfx_Tired;
    ushort field_A36;
    bool32 playingTiredSFX;
    ushort sfx_Land;
    ushort sfx_Slide;
    ushort sfx_Outtahere;
    ushort sfx_Transform2;
    ushort sfx_Swap;
    ushort sfx_SwapFail;
    ushort sfx_MightyDeflect;
    ushort sfx_MightyDrill;
    ushort sfx_MightyLand;
    ushort sfx_MightyUnspin;
    int raySwoopTimer;
    int rayDiveTimer;
    bool32 gotHit[PLAYER_MAX];
    void (*configureGhost_CB)(void);
    bool32 (*field_A6C)(int);
    int field_A70;
} ObjectPlayer;
#else
typedef struct {
    RSDK_OBJECT
    int playerCount;
    int sonicPhysicsTable[64];
    int tailsPhysicsTable[64];
    int knuxPhysicsTable[64];
    ushort upState;
    ushort downState;
    ushort leftState;
    ushort rightState;
    ushort jumpPressState;
    ushort jumpHoldState;
    int flyCarryPosA;
    int flyCarryPosB;
    Vector2 flyCarryPositions[16];
    Vector2 curFlyCarryPos;
    int p2UnknownInputTimer;
    int jumpInDelay;
    int p2InputDelay;
    int rings;
    int ringExtraLife;
    int savedTargetScore;
    int powerups;
    int savedLives;
    int savedScore;
    int savedScore1UP;
    ushort sonicSpriteIndex;
    ushort superSpriteIndex;
    ushort tailsSpriteIndex;
    ushort tailsTailsSpriteIndex;
    ushort knuxSpriteIndex;
    colour superPalette_Sonic[18];
    colour superPalette_Sonic_CPZ[18];
    colour superPalette_Sonic_HCZ[18];
    colour superPalette_Tails[18];
    colour superPalette_Tails_CPZ[18];
    colour superPalette_Tails_HCZ[18];
    colour superPalette_Knux[18];
    colour superPalette_Knux_CPZ[18];
    colour superPalette_Knux_HCZ[18];
    ushort field_65E;
    ushort sfx_Jump;
    ushort sfx_Roll;
    ushort sfx_Charge;
    ushort sfx_Release;
    ushort sfx_PeelCharge;
    ushort sfx_PeelRelease;
    ushort sfx_Dropdash;
    ushort sfx_LoseRings;
    ushort sfx_Hurt;
    ushort field_672;
    ushort sfx_Skidding;
    ushort sfx_Grab;
    ushort sfx_Flying;
    ushort field_67A;
    bool32 playingFlySFX;
    ushort sfx_Tired;
    ushort field_682;
    bool32 playingTiredSFX;
    ushort sfx_Land;
    ushort sfx_Slide;
    ushort sfx_Outtahere;
    ushort sfx_Transform2;
    bool32 gotHit[4];
} ObjectPlayer;
#endif

// Entity Class
typedef struct {
    RSDK_ENTITY
    void (*state)(void);
    void (*nextAirState)(void);
    void (*nextGroundState)(void);
    EntityCamera *camera;
    AnimationData playerAnimData;
    AnimationData tailSpriteAnimData;
    int maxWalkSpeed;
    int maxJogSpeed;
    int maxRunSpeed;
    int field_A4;
    int tailRotation;
    int tailDirection;
    ushort spriteIndex;
    ushort tailSpriteIndex;
    ushort storedAnim;
    ushort playerID;
    Hitbox *outerbox;
    Hitbox *innerbox;
    int characterID;
    int rings;
    int ringExtraLife;
    int shield;
    int lives;
    int score;
    int score1UP;
    int hyperRing;
    int timer;
    int outtaHereTimer;
    int abilityTimer;
    int spindashCharge;
    int abilityValue;
    int airTimer;
    int invincibleTimer;
    int speedShoesTimer;
    int blinkTimer;
    int scrollDelay;
    int skidding;
    int pushing;
    bool32 underwater;
    int groundedStore;
    int invertGravity;
    int isChibi;
    int forceTransform;
    int superState;
    int superSecondTimer;
    int superBlendAmount;
    int superBlendDirection;
    int sidekick;
    int scoreBonus;
    int cameraOffset;
    int killFlagA;
    int killFlagB;
    int topSpeed;
    int acceleration;
    int deceleration;
    int airAcceleration;
    int airDeceleration;
    int skidSpeed;
    int rollingFriction;
    int rollingDeceleration;
    int gravityStrength;
    int abilitySpeed;
    int jumpStrength;
    int jumpCap;
    int flailing;
    int sensorX[5];
    int sensorY;
    Vector2 field_194;
    void (*inputState2)(void);
    void (*inputState)(void);
    int controllerID;
    int glideTimer;
    int up;
    int down;
    int left;
    int right;
    int jumpPress;
    int jumpHold;
    int jumpAbility;
    int jumpAbilityTimer;
    void (*movesetPtr)(void);
    void (*move_Peelout)(void);
    int flyCarryTimer;
    Vector2 sidekickPos;
    Vector2 leaderPos;
    char hurtFlag;
    char field_1E9;
    char field_1EA;
    char field_1EB;
    bool32 forceJumpIn;
#if RETRO_USE_PLUS
    bool32 isGhost;
#endif
    int maxGlideSpeed;
    char field_1F8;
    char field_1F9;
    char field_1FA;
    char field_1FB;
    char field_1FC;
    char field_1FD;
    char field_1FE;
    char field_1FF;
    char field_200;
    char field_201;
    char field_202;
    char field_203;
    char field_204;
    char field_205;
    char field_206;
    char field_207;
    char field_208;
    char field_209;
    char field_20A;
    char field_20B;
    char field_20C;
    char field_20D;
    char field_20E;
    char field_20F;
    char field_210;
    char field_211;
    char field_212;
    char field_213;
    void *entPtr;
    int field_218;
    int field_21C;
#if RETRO_USE_PLUS
    int field_220;
    int field_224;
    int field_228;
    int field_22C;
    int field_22F;
    int field_230;
    int uncurlTimer;
#endif
} EntityPlayer;

// Object Struct
extern ObjectPlayer *Player;

// Standard Entity Events
void Player_Update(void);
void Player_LateUpdate(void);
void Player_StaticUpdate(void);
void Player_Draw(void);
void Player_Create(void* data);
void Player_StageLoad(void);
void Player_EditorDraw(void);
void Player_EditorLoad(void);
void Player_Serialize(void);

// Extra Entity Functions
void Player_LoadSprites(void);
void Player_LoadSpritesVS(void);
void Player_SaveValues(void);
void Player_GiveScore(EntityPlayer *player, int score);
void Player_GiveRings(int amount, EntityPlayer *player, bool32 playSFX);
void Player_GiveLife(EntityPlayer *entity);
void Player_ApplyShieldEffect(EntityPlayer *player);
void Player_ChangeCharacter(EntityPlayer *entity, int character);
bool32 Player_CheckGoSuper(EntityPlayer *player, byte emeraldflags);
bool32 Player_CheckKeyPress(void);
void Player_LoseRings(EntityPlayer *player, int rings, byte cPlane);
void Player_LoseHyperRings(EntityPlayer *player, int rings, byte cPlane);
Entity* Player_Unknown2(void);
Entity *Player_Unknown3(void);
#if RETRO_USE_PLUS
void Player_Unknown4(void);
#endif
void Player_Unknown5(EntityPlayer *player);
void Player_Unknown7(void);
void Player_HandleDeath(EntityPlayer *player);
void Player_ResetState(EntityPlayer *player);

// Collision
Hitbox *Player_GetHitbox(EntityPlayer *player);
bool32 Player_CheckCollisionTouch(EntityPlayer *player, void *entity, Hitbox *entityHitbox);
bool32 Player_CheckCollisionBox(EntityPlayer *player, void *entity, Hitbox *entityHitbox);
bool32 Player_CheckCollisionPlatform(EntityPlayer *player, void *entity, Hitbox *entityHitbox);

bool32 Player_CheckHit(EntityPlayer *player, void *entity);
bool32 Player_CheckBadnikHit(EntityPlayer *player, void *entity, Hitbox *entityHitbox);
bool32 Player_CheckBadnikBreak(void *entity, EntityPlayer *player, bool32 destroy);
bool32 Player_CheckBossHit(EntityPlayer *player, void *entity);
bool32 Player_CheckProjectileHit(EntityPlayer *player, void *projectile);
bool32 Player_CheckHit2(EntityPlayer *player, void *entity, bool32 flag);

//State helpers
void Player_ChangePhysicsState(EntityPlayer *entity);
void Player_HandleGroundMovement(void);
void Player_HandleGroundRotation(void);
void Player_HandleAirMovement(void);
void Player_HandleAirFriction(void);
void Player_StartJump(EntityPlayer *entity);
void Player_StartRoll(void);
bool32 Player_SwapMainPlayer(bool32 flag);
void Player_StartPeelout(void);
void Player_HandleRollDeceleration(void);
void Player_Hit(EntityPlayer *player);
bool32 Player_CheckValidState(EntityPlayer *player);
void Player_CheckFlyCarry(EntityPlayer *player);
void Player_P2JumpBackIn(void);
void Player_Unknown9(void);

//States
void Player_State_None(void);
void Player_State_Ground(void);
void Player_State_Air(void);
void Player_State_Roll(void);
void Player_State_ForceRoll(void);
void Player_State_RollLock(void);
void Player_State_LookUp(void);
void Player_State_Crouch(void);
void Player_State_Spindash(void);
void Player_State_Peelout(void);
void Player_State_OuttaHere(void);
void Player_State_Transform(void);
void Player_State_Hit(void);
void Player_State_Die(void);
void Player_State_Drown(void);
void Player_State_DropDash(void);
void Player_State_BubbleBounce(void);
void Player_State_TailsFlight(void);
void Player_State_FlyCarried(void);
void Player_State_KnuxGlideLeft(void);
void Player_State_KnuxGlideRight(void);
void Player_State_KnuxGlideDrop(void);
void Player_State_GlideSlide(void);
void Player_State_KnuxWallClimb(void);
void Player_State_KnuxLedgePullUp(void);
#if RETRO_USE_PLUS
void Player_State_MightyHammerDrop(void);
void Player_State_MightyUnspin(void);
void Player_SpawnMightyHammerdropDust(int speed, Hitbox *hitbox);
void Player_State_RayGlide(void);
#endif
void Player_State_FlyIn(void);
void Player_State_JumpIn(void);
void Player_State_StartJumpIn(void);
void Player_EndFlyJumpIn(EntityPlayer *thisEntity, EntityPlayer *player);
void Player_State_Unknown(void);
void Player_State_Victory(void);
void Player_State_Bubble(void);
void Player_State_WaterSlide(void);

void Player_SonicJumpAbility(void);
void Player_TailsJumpAbility(void);
void Player_KnuxJumpAbility(void);
#if RETRO_USE_PLUS
void Player_MightyJumpAbility(void);
void Player_RayJumpAbility(void);
#endif

//Input States
void Player_GetP1Inputs(void);
void Player_FlyCarryInputState(void);
void Player_GetP2Inputs(void);
void Player_GetP2NoInput(void);
void Player_GetP2InputUnknown(void);
void Player_GetP2PlayerInputs(void);

#endif //!OBJ_PLAYER_H
