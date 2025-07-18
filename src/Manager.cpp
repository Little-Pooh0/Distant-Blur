#include "Manager.h"
// state enum for main thread to check
enum class State { Pause, Run, Kill };

// Global state to control the main thread
State state = State::Run;
int sleepTime = 100; // Default sleep time in milliseconds

namespace Manager {
	/**
	* @brief The main initialization function for the mod logic.
	* Called from the SKSE plugin entry point after game data is loaded.
	*/
	void Initialize() {
		logger::info("Manager: {} Initializing, Entering and Allocating trampoline.", modName);

		// Allocate trampoline space early. Commonlib handles getting the SKSE trampoline.
		// Requesting reasonable sizes for branch and local (code generation) trampolines.
		// Combined size might need adjustment based on total hooks from all mods.
		SKSE::AllocTrampoline(1 << 10);  // 2^10 = 1024 bytes for branch trampoline
		logger::info("Manager: Trampoline allocated, now Calling ResolveAddresses");

		Utils::JsonHandling::loadJson();

		Hooks::InstallHooks(); // Install all hooks and patches

		logger::info("Manager: Exiting, InstallHooks finished and {} Initialization finished.", modName);
		return;
	}


}