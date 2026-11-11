#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "match.h"
#include "Player/player.h"


using namespace godot;


void Match::_bind_methods(){}


void Match::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    env = get_node<WorldEnvironment>("%Enviroment");
    ring = get_node<CompleteRing>("%CompleteRing");
    player_scene = ResourceLoader::get_singleton()->load("res://Player/player.tscn");
    if(player_scene.is_null()) {return UtilityFunctions::push_error("player_scene: den egine load player scene");}
    players_con = get_node<Node3D>("%Players");
    net_man = get_node<NetworkManager>("/root/NetworkManagerGlobal");
}