#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

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
    menu_sc = get_node<MatchMenu>("%MatchMenu");
    net_man = get_node<NetworkManager>("/root/NetworkManagerGlobal");
    load_sc = get_node<LoadingScreen>("/root/LoadingScreen");
    error_sc = get_node<ErrorPopup>("/root/ErrorPopup");
    bg = Object::cast_to<MenuBackground>(get_node_or_null("/root/MenuBackgroundGlobal"));    
    if(bg) bg->set_active(false);
    if(load_sc) load_sc->show_loading("Connecting all players");
    if(net_man)
    {
        if(!ring || !players_con) return UtilityFunctions::push_error("ta ring nodes kai playercon nodes lupoun");
        setup_match(net_man->get_roster());
        net_man->connect("all_connected",callable_mp(this,&Match::on_all_connected));
        if(net_man->is_all_connected()) on_all_connected();
        net_man->connect("match_ended", callable_mp(this, &Match::on_match_ended));
        net_man->connect("peer_left", callable_mp(this, &Match::on_peer_left));
        net_man->connect("lobby_closed",callable_mp(this,&Match::lobby_closed));
        net_man->connect("disconnected", callable_mp(this, &Match::disconnected));
    }
    if(menu_sc)
    {
        if(net_man)menu_sc->set_is_host(net_man->get_id());
        menu_sc->connect("leave_pressed",callable_mp(this,&Match::leave_pressed));
        menu_sc->connect("stop_game_pressed",callable_mp(this,&Match::stop_match_pressed));
    }
     
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

void Match::on_all_connected()
{
    if(load_sc) load_sc->hide_loading();
}

void Match::leave_pressed()
{
    if(!net_man || !bg) return;
    net_man->leave_lobby();
    load_sc->show_loading("Back To Lobbies List");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_list.tscn");
    bg->set_active(true);
}

void Match::lobby_closed()
{
    if(!load_sc || !error_sc || !bg) return;
    error_sc->createPopup("Host Left");
    load_sc->show_loading("Back to lobbies list");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_list.tscn");
    bg->set_active(true);
}

void Match::on_peer_left(int id)
{
    Node* p = players_con->get_node_or_null(String::num_int64(id));
    if(p) p->queue_free();
}

void Match::stop_match_pressed()
{
    if(net_man) net_man->end_match();
}

void Match::on_match_ended()
{
    if(load_sc) load_sc->show_loading("Back To The Lobby");
    if(error_sc && net_man->get_id() != 1) error_sc->createPopup("Host Stopped Match");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_room.tscn");
    if(bg) bg->set_active(true);
}

void Match::disconnected()
{
    if(!net_man || !error_sc || !load_sc || !bg) return;
    load_sc->show_loading("Back To MainMenu");
    error_sc->createPopup("Servers Down");
    get_tree()->change_scene_to_file("res://UserInterfaces/MainMenu/main_menu.tscn");
    bg->set_active(true);
}