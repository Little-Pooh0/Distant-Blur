#include "Hooks.h"

RE::TESImageSpace* iMod;

namespace Hooks {

	void InstallHooks() {
		logger::info("InstallHooks: Entering.");

		// --- Replace all previous creation logic with our new robust finder ---
		CreateDOFForm();

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

	void CreateDOFForm() {
		if (!iMod) {
			iMod = RE::IFormFactory::Create<RE::TESImageSpace>();

			if (iMod) {
				logger::info("Successfully cloned template. Our private runtime IMOD is ready. FormID: {}", iMod->GetFormID());
				iMod->SetFormEditorID("DistantBlurImageSpaceModifier");
			}
			else {
				logger::error("Failed to clone the found template!");
			}
		}
		return;
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
		RE::ImageSpaceManager::GetSingleton()->GetRuntimeData().currentBaseData = &iMod->data;
		SetIModDefaults();
		InitiateLegacyBlur(*iMod);
		logger::info("UpdateHook: InitiateLegacyBlur called, returning from Update.");
	}

	void SetIModDefaults() {
		using namespace MCP::Advanced::Legacy;
		logger::info("SetIModDefaults: Entering, setting default values for ImageSpaceModifier.");
		if (!iMod) {
			logger::error("SetIModDefaults: iMod is null, cannot set defaults.");
			return;
		}
		iMod->data.hdr.eyeAdaptSpeed = 3.0f; // Default eye adaptation speed
		iMod->data.hdr.eyeAdaptStrength = 3.0f; // Default eye adaptation strength
		iMod->data.hdr.bloomBlurRadius = 7.0f; // Default bloom blur radius
		iMod->data.hdr.bloomThreshold = 0.6f; // Default bloom threshold
		iMod->data.hdr.bloomScale = 0.5f; // Default bloom scale
		iMod->data.hdr.receiveBloomThreshold = 0.15f; // Default receive bloom threshold
		iMod->data.hdr.white = 0.15f; // Default white point
		iMod->data.hdr.sunlightScale = 1.8f; // Default sunlight scale
		iMod->data.hdr.skyScale = 1.5f; // Default sky scale

		iMod->data.cinematic.saturation = 0.9f; // Default saturation
		iMod->data.cinematic.brightness = 1.5f; // Default brightness
		iMod->data.cinematic.contrast = 1.1f; // Default contrast


		iMod->data.depthOfField.strength = 0.0f; // Default strength
		iMod->data.depthOfField.distance = 0.0f; // Default focal point
		iMod->data.depthOfField.range = 0.0f; // Default range
	}



	void InitiateLegacyBlur(RE::TESImageSpace& imageSpace) {
		using namespace MCP::Advanced::Legacy;

		logger::info("InitiateLegacyBlur: Weather Current weather type is: {}", Utils::GetCurrentWeather());

		// Use std::find_if to get an iterator to the matching row.
		auto vectorCurrentWeather = std::find_if(weatherSettings.begin(), weatherSettings.end(), [&](const WeatherSettingRow& setting) {
			return setting.rowWeatherType == Utils::GetCurrentWeather();
		});

		// Check if the weather was actually found in our settings.
		if (vectorCurrentWeather == weatherSettings.end()) {
			return;
		}

		// If we get here, 'vectorCurrentWeather' points to a valid row. Calculate the index.
		int rowOfWeather = (int)std::distance(weatherSettings.begin(), vectorCurrentWeather); // returns __int64, just cast it to int for safety.

		logger::info("InitiateLegacyBlur: Entering with row index: {}", rowOfWeather);
		float strengthToSet = weatherSettings[rowOfWeather].rowBlurStrength;
		float rangeToSet = weatherSettings[rowOfWeather].rowBlurRange;

		logger::info("Setting ImageSpaceModifier strength to: {}", strengthToSet);
		logger::info("Setting ImageSpaceModifier range to: {}", rangeToSet);
		imageSpace.data.depthOfField.strength = strengthToSet;
		imageSpace.data.depthOfField.range = rangeToSet;
		logger::info("Values should be equal {}", imageSpace.data.depthOfField.strength, imageSpace.data.depthOfField.range);

		return;
	}

}