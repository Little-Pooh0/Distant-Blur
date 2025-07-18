#include "MCP.h"


namespace MCP {
	void Register() {
		logger::info("Registering MCP functionality");
		if (!SKSEMenuFramework::IsInstalled()) {
			return;
			logger::error("SKSEMenuFramework is not installed. MCP functionality cannot be registered.");
		}
		//filter = new ImGuiTextFilter();
		SKSEMenuFramework::SetSection("Distant Blur");
		SKSEMenuFramework::AddSectionItem("General Settings", General::Render);
		SKSEMenuFramework::AddSectionItem("Advanced/Legacy", Advanced::Legacy::Render);
		//SKSEMenuFramework::AddSectionItem("Advanced/Universal", Advanced::Universal::Render);
		SKSEMenuFramework::AddSectionItem("Maintenance Settings", Maintenance::Render);
	}
	

	
	namespace General {
		using namespace Settings::General;

		void __stdcall Render() {
			ImGui::Checkbox("Mod Toggle", &ModToggle);
			ImGui::Combo("Item Type", &BlurType, blurNames, IM_ARRAYSIZE(blurNames), IM_ARRAYSIZE(blurNames));
			// Cycling Keybind
			// ImGui::
		}
	}

	namespace Advanced {
		using namespace Settings::Advanced;
		void __stdcall Render() {
			
		}

		namespace Legacy {

			void __stdcall Render() {
				using namespace Settings::Advanced::Legacy;
				if (ImGui::CollapsingHeader("Weather Smoothing Settings##header")) { //, ImGuiTreeNodeFlags_DefaultOpen
					ImGui::Checkbox("Weather Smoothing Toggle", &WeatherSmoothing);
					//ImGui::SliderFloat("Weather Smoothing Strength", &WeatherSmoothingStrength, 0.0f, 1.0f, "%.2f");
				}
				if (ImGui::CollapsingHeader("Interior Settings##header")) { //, ImGuiTreeNodeFlags_DefaultOpen
					ImGui::Checkbox("Interior Blur Toggle", &InteriorBlurToggle);
					ImGui::SliderFloat("Interior Blur Strength", &InteriorBlurStrength, 0.0f, 1.0f, "%.2f");
					ImGui::SliderFloat("Interior Blur Range", &InteriorBlurRange, 0.0f, 100.0f, "%.2f");
					ImGui::Checkbox("Interior Preliminary Patch", &InteriorPreliminaryPatch);
				}
				if (ImGui::CollapsingHeader("Exterior Settings##header")) { //, ImGuiTreeNodeFlags_DefaultOpen
					ImGui::Checkbox("Exterior Blur Toggle", &exteriorBlurToggle);
					ImGui::SliderFloat("Exterior Blur Strength", &exteriorBlurStrength, 0.0f, 1.0f, "%.2f");
					ImGui::SliderFloat("Exterior Blur Range", &exteriorBlurRange, 0.0f, 100.0f, "%.2f");
					ImGui::Checkbox("Exterior Preliminary Patch", &exteriorPreliminaryPatch);
				}
				RenderWeatherTable();
			}

			void RenderWeatherTable() {
				if (!Utils::weathersPopulated) {
					Utils::LookThroughAllWeatherForms(); // Ensure we have the weather names populated
				}
				if (ImGui::CollapsingHeader("Weather Whitelist##header", ImGuiTreeNodeFlags_DefaultOpen)) {
					/*for (numberOfPresets, int i = 0; i < numberOfPresets; i++) {
						ImGui::Text("Preset %d", i + 1);
					}*/
					if (ImGui::BeginTable("weatherTable", 8, tableFlags)) {
						ImGui::TableSetupColumn("Toggle", columnFlags, 75.0f); // can be changed to an array of column widths
						ImGui::TableSetupColumn("Weather", columnFlags, 300.0f);
						ImGui::TableSetupColumn("##Current-Weather", columnFlags, 50.0f);
						ImGui::TableSetupColumn("Strength", columnFlags, 150.0f);
						ImGui::TableSetupColumn("Range", columnFlags, 175.0f);
						ImGui::TableSetupColumn("Static", columnFlags, 75.0f);
						ImGui::TableSetupColumn("Reset", columnFlags, 50.0f);
						ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthStretch);


						// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
						/*auto skyWeather = RE::Sky::GetSingleton()->currentWeather;
						if (skyWeather != nullptr) {
							skyWeatherName = clib_util::editorID::get_editorID(skyWeather);
						} else {
							skyWeatherName = "None";
							logger::warn("Current weather is null, cannot set weather type.");
						}*/
						for (row = 0; row < weatherSettings.size(); row++) {
							// Manually draw the header row
							if (row == 0) {
								ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
								for (int headerColumn = 0; headerColumn < 8; headerColumn++) {
									ImGui::TableSetColumnIndex(headerColumn);
									// Use helper function to draw the centered header text
									Utils::CenteredText(headerTitles[headerColumn]);
								}
							}
							ImGui::PushID(row);
							ImGui::TableNextRow();

							comboPreviewValue = weatherSettings[row].rowWeatherType.c_str();

							for (int column = 0; column < 8; column++) {
								ImGui::TableSetColumnIndex(column);
								switch (column) {
									case 0:
										ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFrameHeight()) * 0.5f);
										ImGui::Checkbox("##Toggle", &weatherSettings[row].rowToggle);
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Enable/Disable this row\n(Default: Enabled)");
										}
										break;
									case 1:
										ImGui::PushItemWidth(-FLT_MIN);
										if (ImGui::BeginCombo("##Weather", comboPreviewValue)) {
											for (int index = 0; index < Utils::gameWeatherNames.size(); index++) {
												const bool isSelected = (weatherSettings[row].rowWeatherType == Utils::gameWeatherNames[index]);

												if (ImGui::Selectable(Utils::gameWeatherNames[index].c_str(), isSelected)) {
													weatherSettings[row].rowWeatherType = Utils::gameWeatherNames[index];
												}
												if (isSelected) {
													ImGui::SetItemDefaultFocus();
												}
											}
											ImGui::EndCombo();
										}
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Select the weather that this rows settings apply to\n(Default: None)");
										}
										break;
									case 2:
										ImGui::PushItemWidth(-FLT_MIN);
										FontAwesome::PushSolid();
										if (ImGui::Button(weatherIcon.c_str(), ImVec2(-FLT_MIN, 0.0f))) {
											logger::info("Weather selection button clicked for row {}", row);
											weatherSettings[row].rowWeatherType = Utils::GetCurrentWeather();
										}
										FontAwesome::Pop();
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Get the current weather and set it for this row");
										}
										break;
									case 3:
										ImGui::PushItemWidth(-FLT_MIN);
										ImGui::InputFloat("##Strength", &weatherSettings[row].rowBlurStrength, 0.01f, 0.1f, "%.2f", inputFlags);
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("The blur strength amplifier for the selected weather\n(Default: 1.00)");
										}
										break;
									case 4:
										ImGui::PushItemWidth(-FLT_MIN);
										ImGui::InputFloat("##Range", &weatherSettings[row].rowBlurRange, 1.0f, 10.0f, "%.2f", inputFlags); // also test with hundreds instead of 10s.
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Blur Range");
										}
										break;
									case 5:
										ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFrameHeight()) * 0.5f);
										ImGui::Checkbox("##Static", &weatherSettings[row].rowStaticToggle);
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Enabled: Blur is static\nDisabled: Blur smooths with weather transitions\n(Default: Disabled)");
										}
										break;
									case 6:
										FontAwesome::PushSolid();
										if (ImGui::Button(defaultsIcon.c_str(), ImVec2(-FLT_MIN, 0.0f))) {
											weatherSettings[row] = WeatherSettingRow{};
										}
										FontAwesome::Pop();
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Reset this row's settings to default");
										}
										break;
									case 7:
										FontAwesome::PushSolid();
										if (ImGui::Button(trashIcon.c_str(), ImVec2(-FLT_MIN, 0.0f))) {
											rowToRemove = row; // Capture the current row index to remove it after the table
										}
										FontAwesome::Pop();
										if (ImGui::IsItemHovered(tooltipFlags)) {
											ImGui::SetTooltip("Remove this row");
										}
										break;
									default:
										ImGui::Text("Unknown Column");
										logger::error("Unknown column index: {}, row {}", column, row);
										break;
								}
							}
							ImGui::PopID();
						}
						ImGui::EndTable();
						if (rowToRemove != -1) {
							weatherSettings.erase(weatherSettings.begin() + rowToRemove);
							rowToRemove = -1; // Reset after being used
						}

						FontAwesome::PushRegular();
						if (ImGui::Button(plusIcon.c_str(), ImVec2(-FLT_MIN, 0.0f))) {
							weatherSettings.push_back(WeatherSettingRow{});
						}
						FontAwesome::Pop();
						if (ImGui::IsItemHovered(tooltipFlags)) {
							ImGui::SetTooltip("Add a new weather configuration row");
						}
					}
				}
			}
		}
	}

	void __stdcall Maintenance::Render() {
		if (ImGui::Button("Save Settings")) {
			Settings::SaveSettings();
		}

		if (ImGui::Button("Reset Settings")) {
			Settings::ResetSettings();
		}
	}
}