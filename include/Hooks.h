#pragma once

#include "PCH.h"  
#include "Settings.h"  
#include "Utils.h"
#include "MCP.h"

namespace Hooks {
    void InstallHooks();
	void InitiateLegacyBlur();
    void FindAndCloneDoFTemplate();

    struct UpdateHook {
        static void Update(RE::Actor* a_this, float a_delta);
        static inline REL::Relocation<decltype(Update)> Update_;
        static void Install();
    };
}