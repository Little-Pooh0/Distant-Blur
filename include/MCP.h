#pragma once

#include "PCH.h"
#include "Settings.h"
#include "Utils.h"
#include "Hooks.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace MCP {
	//inline ImGuiTextFilter* filter;
	static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit; //
	static ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_WidthFixed;
	static ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll;
	static ImGuiHoveredFlags tooltipFlags = ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay;

	void Register();

	namespace General {
		void Render();
		inline const char* blurNames[] = {
			"None", "Legacy", "Universal"
		};
	}

	namespace Advanced {
		namespace Legacy {
			inline int rowToRemove = -1; // Used to track which row to remove after the table is rendered
			inline int row = 0;
			inline const char* headerTitles[] = { "Toggle", "Weather", "", "Strength", "Range", "Static", "Reset", "Remove" };
			inline const char* comboPreviewValue = "Unknown"; // A fallback
			inline int numberOfPresets = 0; // Default number of presets
			struct WeatherSettingRow {
				bool rowToggle = true; // Default to enabled
				std::string rowWeatherType = "None"; // Use std::string for the weather name/EditorID
				float rowBlurStrength = 1.0f;
				float rowBlurRange = 100.0f; // Default Range for blur
				bool rowStaticToggle = false;
			};
			inline std::vector<WeatherSettingRow> weatherSettings = {
			 	{ true, "None", 1.0f, 100.0f, false }, // Default values for first row (will be overwritten when json is loaded)
			};

			void Render();
			void RenderWeatherTable();
			inline std::string weatherIcon = FontAwesome::UnicodeToUtf8(0xe09a) + "##Get-Weather";
			inline std::string defaultsIcon = FontAwesome::UnicodeToUtf8(0xf021) + "##Default";
			inline std::string trashIcon = FontAwesome::UnicodeToUtf8(0xf1f8) + "##Remove";
			inline std::string plusIcon = FontAwesome::UnicodeToUtf8(0xf0fe) + "##Add";
		}
		namespace Universal {
			void Render();
		}
	}

	namespace Maintenance {
		void Render();
	}
}