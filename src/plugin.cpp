#include "PCH.h"
#include "logger.h"
#include "MCP.h"
#include "Manager.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
	// Handle messages from SKSE called in order of first to last occurance
    switch (message->type) {
        case SKSE::MessagingInterface::kPostLoad:
            // Called after all skse plugins have loaded and sent to registered plugins once all plugins have been loaded
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
	    	// Called immediately after all plugins have loaded and is useful for debugging skse plugin incompatibilities
            // Sent right after kPostLoad to facilitate the correct dispatching/registering of messages/listeners
            logger::info("PostPostLoad message received");
            break;
	    case SKSE::MessagingInterface::kPreLoadGame:
	    	// message->dataLen: length of file path, data: char* file path of .ess savegame file
            // sent immediately before savegame is read
            break;
	    case SKSE::MessagingInterface::kPostLoadGame:
            // Post-load game, sent after an attempt to load a saved game has finished
            break;
        case SKSE::MessagingInterface::kSaveGame:
	    	// Called when a save is created
            break;
        case SKSE::MessagingInterface::kDeleteGame:
	    	// Sent right before deleting the .skse cosave and the .ess save
            break;
        case SKSE::MessagingInterface::kInputLoaded:
            // Sent right after game input is loaded, right before the main menu initializes
            break;
        case SKSE::MessagingInterface::kNewGame:
            // Sent after a new game is created, before the game has loaded
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            // Sent after the data handler has loaded all its forms
	    	Manager::Initialize();
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    MCP::Register();
    return true;
}
