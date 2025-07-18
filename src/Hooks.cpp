#include "Hooks.h"

RE::TESImageSpaceModifier* iMod;
//RE::ImageSpaceModifierInstanceForm* iModInstance;
RE::ImageSpaceModifierInstanceForm* iModInstance = nullptr;

namespace Hooks {

	void InstallHooks() {
		logger::info("InstallHooks: Entering.");

		// --- Replace all previous creation logic with our new robust finder ---
		FindAndCloneDoFTemplate();

		// Only install hooks if we successfully created our iMod.
		if (iMod) {
			logger::info("InstallHooks: iMod is ready, installing Update hook.");
			UpdateHook::Install();
			logger::info("InstallHooks: Exiting, hooks installed.");
		}
		else {
			logger::error("InstallHooks: Aborting hook installation because runtime IMOD could not be created.");
		}
	}

	void UpdateHook::Install()
	{
		logger::info("UpdateHook: Entering, installing Update hook.");
		REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
		Update_ = PlayerCharacterVtbl.write_vfunc(0xAD, Update);
		logger::info("UpdateHook: Update hook installed successfully.");
	}

	void UpdateHook::Update(RE::Actor* a_this, float a_delta) {
		logger::info("UpdateHook: installing Update_");
		Update_(a_this, a_delta);
		logger::info("UpdateHook: Update_ called, now checking for blur initiation.");
		InitiateLegacyBlur();
		logger::info("UpdateHook: InitiateLegacyBlur called, returning from Update.");
	}

	void FindAndCloneDoFTemplate() {
		if (iMod) return; // Already created.

		logger::info("Searching for a suitable DoF IMOD template to clone...");
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) return;

		// Iterate through all Image Space Modifiers in the game data.
		for (auto* templateIMOD : dataHandler->GetFormArray<RE::TESImageSpaceModifier>()) {
			// We are looking for a template that has the exact pointers we need.
			if (templateIMOD && templateIMOD->dof.strength && templateIMOD->dof.range) {

				logger::info("Found suitable template IMOD: '{}' (FormID: {:#x})", templateIMOD->GetFormEditorID(), templateIMOD->GetFormID());

				// We found one! Clone it and store it in our global iMod.
				iMod = templateIMOD->CreateDuplicateForm(true, nullptr)->As<RE::TESImageSpaceModifier>();

				if (iMod) {
					logger::info("Successfully cloned template. Our private runtime IMOD is ready.");
					iMod->SetFormEditorID("DistantBlurImageSpaceModifier");
				}
				else {
					logger::error("Failed to clone the found template!");
				}

				// We've found what we need, so we can stop searching.
				return;
			}
		}
		logger::error("FATAL: Could not find any IMOD in the game data suitable for cloning for DoF effects!");
	}
	void InitiateLegacyBlur() {
		using namespace MCP::Advanced::Legacy;

		logger::info("InitiateLegacyBlur: Weather Current weather type is: {}", Utils::GetCurrentWeather());

		// Use std::find_if to get an iterator to the matching row.
		auto vectorCurrentWeather = std::find_if(weatherSettings.begin(), weatherSettings.end(), [&](const WeatherSettingRow& setting) {
			return setting.rowWeatherType == Utils::GetCurrentWeather();
		});

		// Check if the weather was actually found in our settings.
		if (vectorCurrentWeather == weatherSettings.end()) {
			// Not found. We should probably stop any existing effect and then exit.
			if (iModInstance) {
				RE::ImageSpaceModifierInstanceForm::Stop(iMod);
				iModInstance = nullptr;
				logger::info("InitiateLegacyBlur: No matching weather found, stopped existing ImageSpaceModifierInstanceForm.");
			}
			return; // Exit the function.
		}

		// If we get here, 'vectorCurrentWeather' points to a valid row. Calculate the index.
		int rowOfWeather = (int)std::distance(weatherSettings.begin(), vectorCurrentWeather); // returns __int64, just cast it to int for safety.

		logger::info("InitiateLegacyBlur: Entering with row index: {}", rowOfWeather);
		float strengthToSet = weatherSettings[rowOfWeather].rowBlurStrength;
		float rangeToSet = weatherSettings[rowOfWeather].rowBlurRange;

		if (!iMod) {
			logger::error("Cannot initiate blur, base iMod is null!");
			return;
		}
		logger::info("Setting ImageSpaceModifier strength to: {}", strengthToSet);
		iMod->dof.strength->floatValue = strengthToSet; // Create a new NiFloatData with one key.
		//iMod->dof.strength->floatData->keys[0] = RE::NiFloatKey(0.0f, strengthToSet);
		//logger::info("did the thing");
		//logger::info("{}", iMod->data.dof.strength);
		if (iMod->dof.strength && iMod->dof.strength->floatData) {
			// The first key [0] holds the value for non-animated effects.
			iMod->dof.strength->floatData->keys[0].SetValue(strengthToSet);
		}
		else {
			logger::error("iMod->dof.strength or its floatData is null!");
		}

		if (iMod->dof.range && iMod->dof.range->floatData) {
			iMod->dof.range->floatData->keys[0] = RE::NiFloatKey(0.0f, rangeToSet);
		}
		else {
			logger::error("iMod->dof.range or its floatData is null!");
		}
		logger::info("Set runtime IMOD properties: DoF Strength = {}, Range = {}", strengthToSet, rangeToSet);

		if (iModInstance) {
			RE::ImageSpaceModifierInstanceForm::Stop(iMod);
			iModInstance = nullptr; // The old instance is now invalid.
		}
		iModInstance = RE::ImageSpaceModifierInstanceForm::Trigger(iMod, 1.0f, nullptr);
		if (iModInstance) {
			logger::info("Successfully triggered new IMOD instance.");
		} else {
			logger::error("Failed to trigger new IMOD instance.");
		}
		return;
	}

}