#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include "Network/network_manager.h"
#include "RingGen/CompleteRing/complete_ring.h"
#include "UIs/LoadingScreenGUI/loading_screen.h"
#include "UIs/ErrorPopupGUI/ErrorPopup.h"
#include "Match/match_menu.h"
#include "Background/menu_background.h"


namespace godot{
class Match: public Node3D
{
GDCLASS(Match,Node3D);
protected:
    static void _bind_methods();
public:
    void _ready() override;
    void setup_match(const Array& roster);
private:
    WorldEnvironment* env = nullptr;
    CompleteRing* ring = nullptr;
    Ref<PackedScene> player_scene;
    Node3D* players_con = nullptr;
    MatchMenu* menu_sc = nullptr;
    NetworkManager* net_man = nullptr;
    LoadingScreen* load_sc = nullptr;
    ErrorPopup* error_sc = nullptr;
    MenuBackground* bg = nullptr;
    void on_all_connected();
    void leave_pressed();
    void lobby_closed();
    void on_peer_left(int id);
    void stop_match_pressed();
    void on_match_ended();
    void disconnected();

};
    
}