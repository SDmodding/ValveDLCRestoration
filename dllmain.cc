#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <SDK/sdhd_sdk.hh>

namespace InGameOnUpdate
{
    using namespace UFG;

    SDK::Hook gHook;
    void __fastcall Fn(GameState* game_state, float fRealTimeDelta)
    {
        gHook(game_state, fRealTimeDelta);

        if (auto storeFrontTracker = StoreFrontTracker::Instance())
        {
            if (storeFrontTracker->mInventoryType != StoreFront::InventoryType_None) {
                return;
            }

            storeFrontTracker->LoadStoreData(StoreFront::InventoryType_Clothing);

            const qSymbol itemNames[] = {
                0x3BB43BA5,
                0x36F71D7C,
                0x323600CB,
                0x2C7150CE,
                0x28B04D79
            };
            for (auto& name : itemNames)
            {
                auto item = storeFrontTracker->FindItemSlow(name);
                if (!item) {
                    continue;
                }

                storeFrontTracker->AddItemToInventory(item, storeFrontTracker->mInventoryType, false, true);
            }

            storeFrontTracker->UnloadStoreData();
        }
    }
}

int __stdcall DllMain(HMODULE mod, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        if (!SDK::IsValidExecutable())
        {
            MessageBoxA(0, "Only Steam v1.0 executable is compatible!\nVisit 'github.com/SDmodding' for more information.", "SDHD_ValveDLCRestoration", MB_OK | MB_ICONERROR);
            return 0;
        }

        InGameOnUpdate::gHook.I_GameStateInGame(SDK::Hook::GameState_OnEnter, InGameOnUpdate::Fn);
        return 1;
    }

    return 1;
}

