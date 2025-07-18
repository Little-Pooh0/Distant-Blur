#include "Utils.h"

namespace Utils {
	void CenteredText(const char* text) {
		ImVec2 a_vec;
		ImGui::CalcTextSize(&a_vec, text, nullptr, false, 0);
		// Calculate the horizontal position to center the text
		float textX = ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - a_vec.x) * 0.5f;
		// Set the cursor to the calculated position and draw the text
		ImGui::SetCursorPosX(textX);
		ImGui::Text("%s", text);
	}

	std::string GetCurrentWeather() {
		skyWeather = RE::Sky::GetSingleton()->currentWeather;
		if (skyWeather != nullptr) {
			skyWeatherName = clib_util::editorID::get_editorID(skyWeather);
		}
		else {
			skyWeatherName = "None";
			logger::warn("Current weather is null, cannot set weather type.");
		}
		return skyWeatherName;
	}

	void LookThroughAllWeatherForms() {
		logger::info("Populating weather list for ImGui...");
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) {
			weathersPopulated = true; // Prevent trying again if it fails
			return;
		}

		gameWeatherNames.clear();
		// Manually add "None" at index 0 to match your default WeatherSettingRow
		gameWeatherNames.push_back("None");

		// Now add all weathers from the game
		for (auto* weather : dataHandler->GetFormArray<RE::TESWeather>()) {
			// We must check if the weather is valid and has a name.
			auto weatherEditorID = clib_util::editorID::get_editorID(weather);
			if (weather) {
				logger::info("Found weather: {}", weatherEditorID);
				gameWeatherNames.push_back(weatherEditorID);
			}
			else {
				logger::warn("Found a null weather form in the data handler, skipping {}", weatherEditorID);
			}
		}
		weathersPopulated = true;
		logger::info("Finished populating weather list with {} entries.", gameWeatherNames.size());
	}

	namespace JsonHandling {
		using namespace MCP::Advanced::Legacy;
		void saveJson() {
			using namespace rapidjson;

			Document doc;
			doc.SetObject();

			Document::AllocatorType& allocator = doc.GetAllocator();

			Value root(kObjectType);

			Value weatherList(kObjectType); {
				Value ashWeathers(kArrayType);
				for (const auto& setting : weatherSettings) {
					rapidjson::Value rowObject(rapidjson::kObjectType);

					rapidjson::Value weatherVal;
					weatherVal.SetString(setting.rowWeatherType.c_str(), allocator);
					rowObject.AddMember("rowWeather", weatherVal, allocator);
					rowObject.AddMember("blurStrength", setting.rowBlurStrength, allocator);
					rowObject.AddMember("staticToggle", setting.rowStaticToggle, allocator);

					ashWeathers.PushBack(rowObject, allocator);
				}
				weatherList.AddMember("ashWeathers", ashWeathers, allocator);
				root.AddMember("weatherList", weatherList, allocator);
			}

			doc.AddMember("MCP", root, allocator);

			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			doc.Accept(writer);

			// save to mod folder
			if (!std::filesystem::exists(modFolder)) {
				std::filesystem::create_directories(modFolder);
			}

			std::ofstream file(modFolder + "settings.json");
			file << buffer.GetString();
			file.close();
		}

		void loadJson() {
			// CHANGE: Log that we are scanning a directory now.
			logger::info("Loading settings from directory: {}", modFolder);

			// Create a path object from your modFolder string.
			const std::filesystem::path settings_dir{ modFolder };

			// NEW: Check if the directory actually exists before trying to scan it.
			if (!std::filesystem::exists(settings_dir) || !std::filesystem::is_directory(settings_dir)) {
				logger::warn("Settings directory '{}' does not exist or is not a directory. Skipping load.", modFolder);
				return;
			}

			// Clear default settings ONCE before loading from any files.
			weatherSettings.clear();

			// NEW: Loop through each entry in the specified directory.
			for (const auto& entry : std::filesystem::directory_iterator(settings_dir)) {
				// NEW: Check if the entry is a regular file and has a .json extension.
				if (entry.is_regular_file() && entry.path().extension() == ".json") {

					logger::info("-> Processing settings file: {}", entry.path().filename().string());

					// --- Your existing logic for processing one file starts here ---

					// CHANGE: Open the file path provided by the directory entry.
					std::ifstream file(entry.path());
					if (!file.is_open()) {
						logger::warn("Could not open file: {}", entry.path().string());
						continue; // Skip to the next file
					}
					std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
					file.close();

					rapidjson::Document doc;
					doc.Parse(str.c_str());

					if (doc.HasParseError() || !doc.HasMember("MCP")) {
						logger::error("Failed to parse JSON or find 'MCP' root in file: {}", entry.path().filename().string());
						continue; // Skip to the next file
					}

					const auto& mcp = doc["MCP"];
					if (mcp.HasMember("Legacy") && mcp["Legacy"].IsObject()) {
						const auto& advancedLegacy = mcp["Legacy"];
						if (advancedLegacy.HasMember("WeatherSettings") && advancedLegacy["WeatherSettings"].IsArray()) {
							const auto& settingsArray = advancedLegacy["WeatherSettings"].GetArray();

							// Note: We are now APPENDING to weatherSettings, not clearing it here.
							for (const auto& rowObject : settingsArray) {
								if (rowObject.IsObject()) {
									bool isEnabled = rowObject.HasMember("rowToggle") && rowObject["rowToggle"].IsBool() ? rowObject["rowToggle"].GetBool() : true;
									std::string rowWeather = rowObject.HasMember("rowWeather") && rowObject["rowWeather"].IsString() ? rowObject["rowWeather"].GetString() : "None";
									float rowStrength = rowObject.HasMember("blurStrength") && rowObject["blurStrength"].IsFloat() ? rowObject["blurStrength"].GetFloat() : 1.0f;
									float rowRange = rowObject.HasMember("blurRange") && rowObject["blurRange"].IsFloat() ? rowObject["blurRange"].GetFloat() : 100.0f;
									bool isStatic = rowObject.HasMember("staticToggle") && rowObject["staticToggle"].IsBool() ? rowObject["staticToggle"].GetBool() : false;

									weatherSettings.push_back(WeatherSettingRow{ isEnabled, rowWeather, rowStrength, rowRange, isStatic });
								}
							}
						}
					}
				}
			}
			// NEW: A final log message summarizing the result.
			logger::info("Finished loading all settings. Total weather configurations loaded: {}.", weatherSettings.size());
		}
	}
}