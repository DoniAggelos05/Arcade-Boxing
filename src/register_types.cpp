#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>


#include "UIs/MainMenuGUI/main_menu.h"
#include "UIs/SettingsMenuGUI/settings_menu.h"
#include "UIs/LoadingScreenGUI/loading_screen.h"
#include "UIs/ErrorPopupGUI/ErrorPopup.h"
#include "Network/network_manager.h"
#include "UIs/LobbyRow/lobby_row.h"
#include "UIs/LobbyList/LobbyBrowser.h"
#include"UIs/CreateLobbyGUI/CreateLobby.h"
#include "UIs/PlayerRowsGUI/player_row.h"
#include "UIs/LobbyRoomGUI/lobby_room.h"
#include "RingGen/CompleteRing/complete_ring.h"
#include "Background/menu_background.h"
#include "Player/player.h"
#include "Match/match.h"


using namespace godot;

// Καλείται όταν το Godot φορτώνει τη βιβλιοθήκη μας.
// Εδώ "δηλώνουμε" στο Godot κάθε δική μας class, για να εμφανίζεται ως node.
void initialize_arcadeboxing_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(MainMenu);
	GDREGISTER_CLASS(SettingsMenu);
	GDREGISTER_CLASS(LoadingScreen);
	GDREGISTER_CLASS(ErrorPopup);
	GDREGISTER_CLASS(NetworkManager);
	GDREGISTER_CLASS(LobbyRow);
	GDREGISTER_CLASS(LobbyBrowser);
	GDREGISTER_CLASS(CreateLobby);
	GDREGISTER_CLASS(PlayerRow);
	GDREGISTER_CLASS(LobbyRoom);
	GDREGISTER_CLASS(CompleteRing);
	GDREGISTER_CLASS(MenuBackground);
	GDREGISTER_CLASS(Player);
	GDREGISTER_CLASS(Match);
}

// Καλείται όταν το Godot κλείνει.
void uninitialize_arcadeboxing_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Το "σημείο εισόδου": το όνομα αυτής της συνάρτησης γράφεται στο .gdextension,
// και από εδώ το Godot ξεκινάει να φορτώνει τη βιβλιοθήκη.
GDExtensionBool GDE_EXPORT arcadeboxing_library_init(
		GDExtensionInterfaceGetProcAddress p_get_proc_address,
		GDExtensionClassLibraryPtr p_library,
		GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_arcadeboxing_module);
	init_obj.register_terminator(uninitialize_arcadeboxing_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
