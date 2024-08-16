// ---------------------------------------------------------------------
// RSDK Project: Sonic Nexus v5
// Object Description: TitleCard Object
// Object Author: MegAmi
// ---------------------------------------------------------------------

#include "TitleCard.hpp"

using namespace RSDK;

namespace GameLogic
{
RSDK_REGISTER_OBJECT(TitleCard);

void TitleCard::Update(void) { this->state.Run(this); }
void TitleCard::LateUpdate(void) {}
void TitleCard::StaticUpdate(void) {}

void TitleCard::Draw(void)
{
    if (!this->stateDraw.Matches(StateMachine_None))
        this->stateDraw.Run(this);
    else
        this->animator.DrawSprite(NULL, true);
}

void TitleCard::Create(void *data)
{
    this->visible = true;

    if (!sceneInfo->inEditor) {
        if (data)
            this->type = (TitleCardTypes)(VOID_TO_INT(data));

        this->active    = ACTIVE_NORMAL;
        this->drawGroup = 6;

        switch (this->type) {
            case TITLECARD_FADEUP:
                this->timer = 384;
                this->state.Set(&TitleCard::State_FadeUp);
                this->stateDraw.Set(&TitleCard::Draw_Greyscale_BG);
                this->drawFX = FX_SCALE;
                this->animator.SetAnimation(&sVars->aniFrames, 5, true, 0);
                break;
            case TITLECARD_SIDEBAR:
                this->position.x = TO_FIXED(screenInfo->size.x - 116);
                this->position.y = TO_FIXED(-512);
                this->state.Set(&TitleCard::State_SideBar);
                this->animator.SetAnimation(&sVars->aniFrames, 3, true, 0);
                break;
            case TITLECARD_FIRSTWORD:
                this->position.x = TO_FIXED(-538);
                this->position.y = TO_FIXED(32);
                this->state.Set(&TitleCard::State_Word);
                this->stateDraw.Set(&TitleCard::Draw_Word);
                this->animator.SetAnimation(&sVars->aniFrames, 0, true, 0);
                break;
            case TITLECARD_SECONDWORD:
                this->position.x = TO_FIXED(-602);
                this->position.y = TO_FIXED(90);
                this->state.Set(&TitleCard::State_Word);
                this->stateDraw.Set(&TitleCard::Draw_Word);
                this->animator.SetAnimation(&sVars->aniFrames, 0, true, 0);
                break;
            case TITLECARD_ZONE:
                this->position.x = TO_FIXED(screenInfo->size.x * 2);
                this->position.y = TO_FIXED(146);
                this->drawFX     = FX_SCALE;
                this->state.Set(&TitleCard::State_Zone);
                this->animator.SetAnimation(&sVars->aniFrames, 1, true, 0);
                break;
            case TITLECARD_ACTBALL:
                this->position.x = TO_FIXED(-152);
                this->position.y = TO_FIXED(-896);
                this->rotation   = 80;
                this->drawFX     = FX_ROTATE;
                this->state.Set(&TitleCard::State_ActBall);
                this->animator.SetAnimation(&sVars->aniFrames, 2, true, 0);
                break;
            case TITLECARD_VERTTEXT1:
                this->position.x = TO_FIXED(24);
                this->position.y = TO_FIXED(0);
                this->inkEffect  = INK_BLEND;
                this->state.Set(&TitleCard::State_VertText_Down);
                this->stateDraw.Set(&TitleCard::Draw_VertText);
                this->animator.SetAnimation(&sVars->aniFrames, 4, true, 0);
                break;
            case TITLECARD_VERTTEXT2:
                this->position.x = TO_FIXED(screenInfo->size.x - 160);
                this->position.y = TO_FIXED(-256);
                this->inkEffect  = INK_BLEND;
                this->state.Set(&TitleCard::State_VertText_Up);
                this->stateDraw.Set(&TitleCard::Draw_VertText);
                this->animator.SetAnimation(&sVars->aniFrames, 4, true, 0);
                break;
        }
    }
}

void TitleCard::StageLoad(void)
{
    sVars->aniFrames.Load("NexusGlobals/TitleCard.bin", SCOPE_GLOBAL);

    // I have to do this instead of just loading because v5's LoadPalette function sucks
    paletteBank[1].Load("TCardTint.act", 0b0000010000000000);
    paletteBank[0].Copy(1, 80, 80, 17);

    // This is the same code from Mania Plus's pause menu
    // I could modify it so the tint matches Nexus better but I'm too dumb to know how to do that LOL
    for (int32 i = 0; i < 0x10000; ++i) {
        uint32 r = (0x20F * (i >> 11) + 23) >> 6;
        uint32 g = (0x103 * ((i >> 5) & 0x3F) + 33) >> 6;
        uint32 b = (0x20F * (i & 0x1F) + 23) >> 6;

        int32 brightness = MIN(((b + g + r) << 8) / 680, 0xFF);

        sVars->tintLookupTable[i] = (brightness >> 3) | ((brightness >> 2) << 5) | ((brightness >> 3) << 11);
    }
    Palette::SetTintLookupTable(sVars->tintLookupTable);

    // SetBlendTable(128, 1, 16, 80);

    foreach_all(TitleCard, titleCard) { titleCard->Copy(RSDK_GET_ENTITY_GEN(SLOT_TITLECARD), true); }
}

void TitleCard::State_FadeUp(void)
{
    foreach_active(Player, player)
    {
        player->controlMode = Player::CONTROLMODE_NONE;
        player->up          = false;
        player->down        = false;
        player->left        = false;
        player->right       = false;
        player->jumpPress   = false;
        player->jumpHold    = false;
    }

    if (this->timer) {
        if (this->timer == 256) {
            Music::Play(Music::TRACK_STAGE);

            GameObject::Reset(SLOT_TITLECARD_VERTTEXT1, sVars->classID, INT_TO_VOID(TITLECARD_VERTTEXT1));
            GameObject::Reset(SLOT_TITLECARD_VERTTEXT2, sVars->classID, INT_TO_VOID(TITLECARD_VERTTEXT2));
            GameObject::Reset(SLOT_TITLECARD_SIDEBAR, sVars->classID, INT_TO_VOID(TITLECARD_SIDEBAR));
            GameObject::Reset(SLOT_TITLECARD_FIRSTWORD, sVars->classID, INT_TO_VOID(TITLECARD_FIRSTWORD));
            GameObject::Reset(SLOT_TITLECARD_SECONDWORD, sVars->classID, INT_TO_VOID(TITLECARD_SECONDWORD));
            GameObject::Reset(SLOT_TITLECARD_ZONE, sVars->classID, INT_TO_VOID(TITLECARD_ZONE));
            GameObject::Reset(SLOT_TITLECARD_ACTBALL, sVars->classID, INT_TO_VOID(TITLECARD_ACTBALL));

            RSDK_GET_ENTITY(SLOT_TITLECARD_ZONE, TitleCard)->animator.frameID = this->actID;

            TitleCard *sideBar  = RSDK_GET_ENTITY(SLOT_TITLECARD_SIDEBAR, TitleCard);
            TitleCard *actBall  = RSDK_GET_ENTITY(SLOT_TITLECARD_ACTBALL, TitleCard);
            actBall->position.x = sideBar->position.x - TO_FIXED(460);
        }

        this->timer -= 8;
    }
    else
        this->state.Set(StateMachine_None);
}

void TitleCard::State_SideBar(void)
{
    if (this->position.y < 0)
        this->position.y += TO_FIXED(16);
}

void TitleCard::State_Word(void)
{
    TitleCard *sideBar   = RSDK_GET_ENTITY(SLOT_TITLECARD_SIDEBAR, TitleCard);
    TitleCard *titleCard = RSDK_GET_ENTITY(SLOT_TITLECARD, TitleCard);

    int32 offset = sideBar->position.x - TO_FIXED(titleCard->offset);
    this->position.x += TO_FIXED(16);
    if (this->position.x > offset)
        this->position.x = offset;
}

void TitleCard::State_Zone(void)
{
    int32 targetPos = TO_FIXED(screenInfo->size.x - 148);
    if (this->position.x > targetPos) {
        this->position.x -= TO_FIXED(8);
        if (this->position.x < targetPos)
            this->position.x = targetPos;
        this->scale.x = ((FROM_FIXED(this->position.x) - 172) << 4) + 512;
        this->scale.y = this->scale.x;
    }
}

void TitleCard::State_ActBall(void)
{
    if (this->timer < 2) {
        this->rotation = (this->rotation + 16) & 511;

        this->velocity.y += 24576;
        this->position.y += this->velocity.y;
        if (this->position.y > TO_FIXED(136)) {
            this->position.y = TO_FIXED(136);
            this->velocity.y = -this->velocity.y / 3;
            if (++this->timer == 2) {
                TitleCard *titleCard = RSDK_GET_ENTITY(SLOT_TITLECARD, TitleCard);
                titleCard->state.Set(&TitleCard::State_Colour_Circle);
                titleCard->stateDraw.Set(&TitleCard::Draw_Colour_Circle);
                titleCard->scale     = { 0, 0 };
                titleCard->inkEffect = INK_TINT;
            }
        }
        this->position.x += TO_FIXED(4);
    }
}

void TitleCard::State_VertText_Down(void)
{
    this->position.x += TO_FIXED(1);
    this->position.y += TO_FIXED(2);
    if (this->position.y > 0)
        this->position.y -= TO_FIXED(516);
}

void TitleCard::State_VertText_Up(void)
{
    this->position.x -= TO_FIXED(1);
    this->position.y -= TO_FIXED(2);
    if (this->position.y < TO_FIXED(-516))
        this->position.y += TO_FIXED(516);
}

void TitleCard::State_Colour_Circle(void)
{
    if (this->scale.x < 1408) {
        this->scale.x += 12;
        this->scale.y = this->scale.x;
    }
    else {
        RSDK_GET_ENTITY(SLOT_TITLECARD_SIDEBAR, TitleCard)->state.Set(&TitleCard::State_SideBar_Exit);
        RSDK_GET_ENTITY(SLOT_TITLECARD_FIRSTWORD, TitleCard)->state.Set(&TitleCard::State_FirstWord_Exit);
        RSDK_GET_ENTITY(SLOT_TITLECARD_SECONDWORD, TitleCard)->state.Set(&TitleCard::State_SecondWord_Exit);
        RSDK_GET_ENTITY(SLOT_TITLECARD_ZONE, TitleCard)->state.Set(&TitleCard::State_Zone_Exit);
        RSDK_GET_ENTITY(SLOT_TITLECARD_ACTBALL, TitleCard)->state.Set(&TitleCard::State_ActBall_Exit);
    }
}

void TitleCard::State_SideBar_Exit(void)
{
    RSDK_GET_ENTITY(SLOT_TITLECARD_VERTTEXT1, TitleCard)->position.x += TO_FIXED(7);
    RSDK_GET_ENTITY(SLOT_TITLECARD_VERTTEXT2, TitleCard)->position.x -= TO_FIXED(7);
    if (this->position.y > TO_FIXED(-192))
        this->position.y -= TO_FIXED(16);
}

void TitleCard::State_FirstWord_Exit(void)
{
    int32 offset = TO_FIXED(screenInfo->center.x - RSDK_GET_ENTITY(SLOT_TITLECARD, TitleCard)->offset - 64);
    if (RSDK_GET_ENTITY(SLOT_TITLECARD_SECONDWORD, TitleCard)->position.x < offset)
        this->position.x -= TO_FIXED(16);
}

void TitleCard::State_SecondWord_Exit(void) { this->position.x -= TO_FIXED(16); }

void TitleCard::State_Zone_Exit(void)
{
    this->position.x += TO_FIXED(8);
    if (this->position.x > TO_FIXED(screenInfo->size.x + 64)) {
        // I have to do this instead of just loading because v5's LoadPalette function sucks
        paletteBank[1].Load("BShieldTint.act", 0b0000010000000000);
        paletteBank[0].Copy(1, 80, 80, 17);
        // SetBlendTable(128, 1, 16, 80);

        foreach_active(Player, player) player->controlMode = Player::CONTROLMODE_PLAYER1;

        for (int32 e = SLOT_TITLECARD; e < SLOT_TITLECARD_ACTBALL; e++) RSDK_GET_ENTITY(e, TitleCard)->Destroy();
        // GameObject::Reset(SLOT_HUD, HUD::sVars->classID, NULL);
    }
}

void TitleCard::State_ActBall_Exit(void)
{
    this->rotation = (this->rotation + 32) & 511;
    this->position.x += 524288;
}

void TitleCard::Draw_Greyscale_BG(void)
{
    Graphics::DrawRect(0, 0, screenInfo->size.x, screenInfo->size.y, 0x000000, 0xFF, INK_TINT, true);
    if (this->timer)
        Graphics::FillScreen(0x000000, this->timer, this->timer, this->timer);
}

void TitleCard::Draw_Colour_Circle(void)
{
    int32 scaleTemp0 = (128 * this->scale.x) >> 9;
    int32 scaleTemp5 = (256 * this->scale.x) >> 9;
    int32 scaleTemp1 = screenInfo->center.x - scaleTemp0;
    int32 scaleTemp2 = scaleTemp1 + scaleTemp5;
    int32 scaleTemp3 = screenInfo->center.y - scaleTemp0;
    int32 scaleTemp4 = scaleTemp3 + scaleTemp5;

    Graphics::DrawRect(0, 0, scaleTemp1, screenInfo->size.y, 0x000000, 0xFF, INK_TINT, true);
    Graphics::DrawRect(scaleTemp2, 0, screenInfo->size.x, screenInfo->size.y, 0x000000, 0xFF, INK_TINT, true);
    Graphics::DrawRect(scaleTemp1, 0, scaleTemp5, scaleTemp3, 0x000000, 0xFF, INK_TINT, true);
    Graphics::DrawRect(scaleTemp1, scaleTemp4, scaleTemp5, screenInfo->size.y, 0x000000, 0xFF, INK_TINT, true);

    RSDK::Vector2 pos;
    pos.x = TO_FIXED(screenInfo->center.x);
    pos.y = TO_FIXED(screenInfo->center.y);
    this->animator.DrawSprite(&pos, true);
}

void TitleCard::Draw_Word(void)
{
    RSDK::Vector2 pos = this->position;
    RSDK::String *word;
    if (this->type == TITLECARD_FIRSTWORD)
        word = &RSDK_GET_ENTITY(SLOT_TITLECARD, TitleCard)->word1;
    else
        word = &RSDK_GET_ENTITY(SLOT_TITLECARD, TitleCard)->word2;

    if (!word || !word->length)
        return;

    // this code probably sucks but fuck it we ball
    for (int32 c = 0; c < word->length; c++) {
        for (int32 f = 0; f < this->animator.frameCount; f++) {
            this->animator.frameID = f;
            if (this->animator.GetFrameID() == word->chars[c])
                break;
        }
        if (this->type == TITLECARD_FIRSTWORD && word->chars[c] >= 'a' && word->chars[c] <= 'z') {
            pos.y += TO_FIXED(13);
            this->animator.DrawSprite(&pos, true);
            pos.y -= TO_FIXED(13);
        }
        else
            this->animator.DrawSprite(&pos, true);
        pos.x += TO_FIXED(this->animator.GetFrame(sVars->aniFrames)->width - 2);
    }
}

void TitleCard::Draw_VertText(void)
{
    RSDK::Vector2 pos = this->position;

    this->animator.animationID = 0;
    this->animator.DrawSprite(&pos, true);
    this->animator.animationID = 1;
    this->animator.DrawSprite(&pos, true);

    pos.y += TO_FIXED(516);

    this->animator.animationID = 0;
    this->animator.DrawSprite(&pos, true);
    this->animator.animationID = 1;
    this->animator.DrawSprite(&pos, true);
}

#if GAME_INCLUDE_EDITOR
void TitleCard::EditorDraw(void)
{
    this->animator.SetAnimation(sVars->aniFrames, 0, true, 1);
    this->animator.DrawSprite(NULL, false);
}

void TitleCard::EditorLoad(void)
{
    sVars->aniFrames.Load("NexusGlobals/Editor.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(TitleCard::sVars, actID);
    RSDK_ENUM_VAR("Act 1", ACT_1);
    RSDK_ENUM_VAR("Act 2", ACT_2);
    RSDK_ENUM_VAR("Act 3", ACT_3);
}
#endif

#if RETRO_REV0U
void TitleCard::StaticLoad(Static *sVars) { RSDK_INIT_STATIC_VARS(TitleCard); }
#endif

void TitleCard::Serialize(void)
{
    RSDK_EDITABLE_VAR(TitleCard, VAR_STRING, word1);
    RSDK_EDITABLE_VAR(TitleCard, VAR_STRING, word2);
    RSDK_EDITABLE_VAR(TitleCard, VAR_UINT8, offset);
    RSDK_EDITABLE_VAR(TitleCard, VAR_UINT8, actID);
}

} // namespace GameLogic