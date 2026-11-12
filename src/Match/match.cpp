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
    Node* background = get_node_or_null("/root/MenuBackgroundGlobal");
    if(background) background->queue_free();    
}


void Match::setup_match(const Array& roster)
{
    Array pl_list;
    for(int i=0;i<roster.size();i++)
    {
        Dictionary d = roster[i];
        int id = d.get("id",0);
        int c = d.get("character",0);
        Player* p = Object::cast_to<Player>(player_scene->instantiate());
        if(!p){UtilityFunctions::push_error("p:Player scene den bwrese na ginei instance"); continue;}
        p->set_name(String::num_int64(id));
        players_con->add_child(p);
        p->set_plr_model(c);
        pl_list.push_back(p);
    }
    ring->build_ring(pl_list.size());
    ring->make_playable(pl_list);
}