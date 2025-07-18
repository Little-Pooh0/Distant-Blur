#include "Settings.h"

namespace Settings {
    void LoadSettings() {
        CSimpleIniW ini;
        ini.SetUnicode();

        /*if (!std::filesystem::exists(jsonFolder)) {
            logger::info("settings.json not found, creating default settings.json");
            return MCP::to_json();
        }*/
        //MCP::Advanced::Legacy::from_json();

        if (!std::filesystem::exists(settingPath)) {
            logger::info("No {} file found, creating new with default values", settingPath);
            
            ResetSettings();
            SaveSettings();
        } else {
            ini.LoadFile(settingPath);

            General::ModToggle = ini.GetBoolValue(L"General", L"ModToggle", General::ModToggle);
            //ini.SetBoolValue(L"General", L"ModToggle", General::ModToggle, L"; Main Mod Toggle");

			General::BlurType = ini.GetLongValue(L"General", L"BlurType", General::BlurType); // add string conversion to int for enhanced readability
			//ini.SetLongValue(L"General", L"BlurType", General::BlurType, L"; Blur Type (0: None, 1: Legacy, 2: Universal)");

            //ini.SaveFile(settingPath);
            logger::info("Settings Loaded");
        }
    }

    void SaveSettings() {
        CSimpleIniW ini;
        ini.SetUnicode();
        ini.LoadFile(settingPath);
        ini.Reset();

        ini.SetBoolValue(L"General", L"ModToggle", General::ModToggle, L"; Main Mod Toggle");
        ini.SetLongValue(L"General", L"BlurType", General::BlurType, L"; Blur Type (0: None, 1: Legacy, 2: Universal)");

        ini.SaveFile(settingPath);
        logger::info("Settings Saved");
	}

    void ResetSettings() {
        General::ModToggle = true;
		General::BlurType = 2;
        
        SaveSettings();
        logger::info("Settings Reset");
    }

}