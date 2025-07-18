#pragma once

#include <CLIBUtil/utils.hpp>
#include <magic_enum.hpp>
#include <iostream>
#include <SKSEMCP/SKSEMenuFramework.hpp>
#include "MCP.h"
// Taken from ImGui.h
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!

const auto modName = static_cast<std::string>(SKSE::PluginDeclaration::GetSingleton()->GetName());
const std::string modFolder = "Data/SKSE/Plugins/" + modName + "/";
//inline auto jsonFolder = modFolder + "settings.json";

namespace Utils {


	inline RE::TESWeather* skyWeather = nullptr;
	inline std::string skyWeatherName = "None"; // Default value for the current weather name
	std::string GetCurrentWeather();

	inline bool weathersPopulated = false; // Flag to check if we have populated the weather list
	inline std::vector<std::string> gameWeatherNames;
	void LookThroughAllWeatherForms();

	void CenteredText(const char* text);
	namespace JsonHandling {
		void saveJson();
		void loadJson();
	}
}