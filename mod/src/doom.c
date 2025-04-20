#include "modding.h"
#include "global.h"
#include "sys_ucode.h"
#include "overlays/actors/ovl_En_Osn/z_en_osn.h"

#include "gLinkHumanSkelGamingAnim.h"
#include "gLinkHumanSkelSitting_on_pcAnim.h"

void Player_Action_Idle(Player* this, PlayState* play);
void Player_Anim_PlayOnce(PlayState*, Player*, PlayerAnimationHeader*);
void Player_Anim_PlayLoop(PlayState*, Player*, PlayerAnimationHeader*);
s32 Player_SetAction(PlayState*, Player*, PlayerActionFunc, s32);

RECOMP_IMPORT("*", void* recomp_alloc(unsigned long size));
RECOMP_IMPORT("*", void recomp_free(void* memory));
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT(".", void DoomDLL_Initialize());
RECOMP_IMPORT(".", void DoomDLL_Tick());
RECOMP_IMPORT(".", void DoomDLL_Input(unsigned int doomKey, unsigned int pressed));
RECOMP_IMPORT(".", void DoomDLL_ScreenCopy(void *dstScreenBuffer));
RECOMP_IMPORT(".", unsigned int DoomDLL_ScreenWidth());
RECOMP_IMPORT(".", unsigned int DoomDLL_ScreenHeight());

static unsigned char *s_DoomScreenBuffer = NULL;
static unsigned int s_DoomScreenWidth = 0;
static unsigned int s_DoomScreenHeight = 0;
static int s_DoomInitialized = false;

#define DOOM_WIDTH 320
#define DOOM_HEIGHT 200

u8 doom_fbs[2][DOOM_WIDTH * DOOM_HEIGHT * 4];
int cur_fb = 0;
int locking_in = 0;
int locked_in = 0;
int desk_visible = 0;

extern Gfx pc_and_desk[];

Player* gPlayer;
PlayState* gPlay;

RECOMP_HOOK("Player_Draw") void on_player_draw(Actor* thisx, PlayState* play) {
    gPlayer = (Player*)thisx;
    gPlay = play;
}

RECOMP_HOOK_RETURN("Player_Draw") void after_player_draw() {
    if (!desk_visible) {
        return;
    }
    
    Player* player = gPlayer;
    Actor* actor = &gPlayer->actor;
    PlayState* play = gPlay;

    OPEN_DISPS(play->state.gfxCtx);

    Vec3s rot = actor->shape.rot;
    float cosang = Math_CosS(rot.y);
    float sinang = Math_SinS(rot.y);

    float x_offset = sinang * 0.0f;
    float y_offset = 12.45f;
    float z_offset = cosang * 0.0f;
    
    Matrix_Push();
    Matrix_SetTranslateRotateYXZ(actor->world.pos.x + x_offset, actor->world.pos.y + actor->shape.yOffset * actor->scale.y + y_offset, actor->world.pos.z + z_offset, &rot);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

	gDPLoadSync(POLY_OPA_DISP++);
	gDPSetTextureImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_32b, 320, doom_fbs[cur_fb]);
    
    DoomDLL_Tick();
    DoomDLL_ScreenCopy(doom_fbs[cur_fb]);

    gSPDisplayList(POLY_OPA_DISP++, pc_and_desk);

    cur_fb ^= 1;

    CLOSE_DISPS(play->state.gfxCtx);
    Matrix_Pop();
}

Vec3f locking_in_at_current = { 0, 0, 0 };
Vec3f locking_in_eye_current = { 0, 0, 0 };
Vec3f locking_in_at_target = { 0, 0, 0 };
Vec3f locking_in_eye_target = { 0, 0, 0 };

void Player_Action_LockedIn(Player* this, PlayState* play) {
    const float Scale = 0.33f;
    const float Step = 5.0f;
    Math_ApproachF(&locking_in_at_current.x, locking_in_at_target.x, Scale, Step);
    Math_ApproachF(&locking_in_at_current.y, locking_in_at_target.y, Scale, Step);
    Math_ApproachF(&locking_in_at_current.z, locking_in_at_target.z, Scale, Step);
    Math_ApproachF(&locking_in_eye_current.x, locking_in_eye_target.x, Scale, Step);
    Math_ApproachF(&locking_in_eye_current.y, locking_in_eye_target.y, Scale, Step);
    Math_ApproachF(&locking_in_eye_current.z, locking_in_eye_target.z, Scale, Step);
    Play_SetCameraAtEye(play, CAM_ID_NONE, &locking_in_at_current, &locking_in_eye_current);
    PlayerAnimation_Update(play, &this->skelAnime);
}

#define DOOMKEY_RIGHTARROW  0xae
#define DOOMKEY_LEFTARROW   0xac
#define DOOMKEY_UPARROW     0xad
#define DOOMKEY_DOWNARROW   0xaf
#define DOOMKEY_STRAFE_L    0xa0
#define DOOMKEY_STRAFE_R    0xa1
#define DOOMKEY_USE         0xa2
#define DOOMKEY_FIRE        0xa3
#define DOOMKEY_ESCAPE      27
#define DOOMKEY_ENTER       13

int stick_to_key(s8 axis, s8 y_axis) {
    if (axis > 64) {
        return y_axis ? DOOMKEY_UPARROW : DOOMKEY_RIGHTARROW;
    }
    else if (axis < -64) {
        return y_axis ? DOOMKEY_DOWNARROW : DOOMKEY_LEFTARROW;
    }
    else {
        return 0;
    }
}

void send_key_if_not_zero(int key, int pressed) {
    if (key != 0) {
        DoomDLL_Input(key, pressed);
    }
}

void send_key_press_or_release(Input *input, int btn, int key) {
    if (CHECK_BTN_ALL(input->press.button, btn)) {
        DoomDLL_Input(key, 1);
    }
    else if (CHECK_BTN_ALL(input->rel.button, btn)) {
        DoomDLL_Input(key, 0);
    }
}

RECOMP_HOOK("Player_ProcessControlStick") void after_player_process_control_stick(PlayState* play, Player* this) {
    if (locked_in) {
        Input *input = CONTROLLER1(&play->state);
        if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
            locked_in = 0;
            desk_visible = 0;
            Player_Anim_PlayOnce(play, this, &gLinkHumanSkelGamingAnim);
            Player_SetAction(play, this, Player_Action_Idle, 1);
        }
        else {
            int prev_x_key = stick_to_key(input->prev.stick_x, 0);
            int cur_x_key = stick_to_key(input->cur.stick_x, 0);
            int prev_y_key = stick_to_key(input->prev.stick_y, 1);
            int cur_y_key = stick_to_key(input->cur.stick_y, 1);
            if (prev_x_key != cur_x_key) {
                send_key_if_not_zero(prev_x_key, 0);
                send_key_if_not_zero(cur_x_key, 1);
            }

            if (prev_y_key != cur_y_key) {
                send_key_if_not_zero(prev_y_key, 0);
                send_key_if_not_zero(cur_y_key, 1);
            }

            send_key_press_or_release(input, BTN_A, DOOMKEY_ENTER);
            send_key_press_or_release(input, BTN_CDOWN, DOOMKEY_USE);
            send_key_press_or_release(input, BTN_B, DOOMKEY_FIRE);
            send_key_press_or_release(input, BTN_CUP, DOOMKEY_ESCAPE);
            send_key_press_or_release(input, BTN_CLEFT, DOOMKEY_STRAFE_L);
            send_key_press_or_release(input, BTN_CRIGHT, DOOMKEY_STRAFE_R);
        }
    }
    else if (locking_in > 0) {
        locking_in--;
        
        if (locking_in == 0) {
            Player_Anim_PlayLoop(play, this, &gLinkHumanSkelGamingAnim);
            locked_in = 1;
        }
    }
    else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_L)) {
        if (!s_DoomInitialized) {
            DoomDLL_Initialize();
            s_DoomInitialized = 1;
        }

        desk_visible = 1;
        Camera *camera = Play_GetCamera(play, CAM_ID_NONE);
        locking_in_at_current = camera->at;
        locking_in_eye_current = camera->eye;
        locking_in_at_target = this->actor.world.pos;
        locking_in_eye_target = this->actor.world.pos;

        const s16 deg_offset = 0;
        f32 rot_cos = Math_CosS(this->actor.world.rot.y + deg_offset);
        f32 rot_sin = Math_SinS(this->actor.world.rot.y + deg_offset);
        Vec3f at_offset = { 8.05f, 21.82f, 27.14f };
        Vec3f eye_offset = { -61.94f * 0.5f, 28.56f, -20.00f * 0.5f };

        locking_in_at_target.x += at_offset.x * rot_cos + at_offset.z * rot_sin;
        locking_in_at_target.y += at_offset.y;
        locking_in_at_target.z += at_offset.z * rot_cos - at_offset.x * rot_sin;

        locking_in_eye_target.x += eye_offset.x * rot_cos + eye_offset.z * rot_sin;
        locking_in_eye_target.y += eye_offset.y;
        locking_in_eye_target.z += eye_offset.z * rot_cos - eye_offset.x * rot_sin;

        Player_SetAction(play, this, Player_Action_LockedIn, 1);
        Player_Anim_PlayOnce(play, this, &gLinkHumanSkelSitting_on_pcAnim);
        locking_in = 21;
    }
}
