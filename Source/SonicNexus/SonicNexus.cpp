#include "SonicNexus.hpp"
#include "Math.hpp"

using namespace RSDK;

#if RETRO_USE_MOD_LOADER
extern "C" {
DLLExport bool32 LinkModLogic(EngineInfo *info, const char *id);
}

const char *modID = "SonicNexus";
#endif

GlobalVariables::Constructor c;
GlobalVariables *globals = nullptr;

void GlobalVariables::Init(void *g)
{
    GlobalVariables *globals = (GlobalVariables *)g;
    memset(globals, 0, sizeof(GlobalVariables));

    sceneInfo->debugMode = false;

    GameLogic::CalculateTrigAngles();
}

#if RETRO_USE_MOD_LOADER
#include "PublicFunctions.hpp"

void InitModAPI(void) { GameLogic::InitPublicFunctions(); }

void InitModAPI(void);
#endif
