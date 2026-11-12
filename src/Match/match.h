#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include "Network/network_manager.h"
#include "RingGen/CompleteRing/complete_ring.h"
#include "UIs/LoadingScreenGUI/loading_screen.h"


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
    NetworkManager* net_man = nullptr;
    LoadingScreen* load_sc = nullptr;
    void on_all_connected();

};
    
}