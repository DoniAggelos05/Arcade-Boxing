#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>



#include "lobby_room.h"
#include "UIs/PlayerRowsGUI/player_row.h"
#include "Characters/characters.h"

using namespace godot;


void LobbyRoom::_bind_methods(){}


void LobbyRoom::_ready()
{

    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    my_id = 0;
    max_player = 0;
    player_rows_body = get_node<VBoxContainer>("%PlayerList");
    row_player = ResourceLoader::get_singleton()->load("res://UserInterfaces/Lobbies/PlayerRow.tscn");
    if (row_player.is_null()) UtilityFunctions::push_error("LobbyRoom: δεν φορτώθηκε το PlayerRow.tscn");
    curr_player_count = get_node<Label>("%PlayerCount");
    lobby_name = get_node<Label>("%LobbyName");
    lobby_code = get_node<Label>("%LobbyCode");
    waiting_text = get_node<Label>("%WaitingText");
    is_priv_toggle = get_node<CheckButton>("%PrivacyToggle");
    on_start_butt = get_node<Button>("%StartGameButton");
    error_sc = get_node<ErrorPopup>("/root/ErrorPopup");
    load_sc = get_node<LoadingScreen>("/root/LoadingScreen");
    net_man = get_node<NetworkManager>("/root/NetworkManagerGlobal");
    Button* leave_butt = get_node<Button>("%LeaveButton");

    for (const char* path : CHARACTER_IMAGES)
    {
        Ref<Texture2D> tex = ResourceLoader::get_singleton()->load(path);
        if (tex.is_valid())
        {
            char_textures.push_back(tex);
        }
        else
        {
            UtilityFunctions::push_error(String("Δεν φορτώθηκε η εικόνα: ") + path);
        }
    }

    if(leave_butt) leave_butt->connect("pressed",callable_mp(this,&LobbyRoom::on_leave_pressed));
    if(net_man){
        net_man->connect("lobby_closed",callable_mp(this,&LobbyRoom::on_lobby_closed));
        net_man->connect("disconnected",callable_mp(this,&LobbyRoom::on_disconnect));
        net_man->connect("peer_joined",callable_mp(this,&LobbyRoom::on_player_joined));
        net_man->connect("peer_left",callable_mp(this,&LobbyRoom::on_player_left));
        net_man->connect("lock_changed",callable_mp(this,&LobbyRoom::on_lock_changed));
        net_man->connect("kicked", callable_mp(this, &LobbyRoom::on_kicked));
    }
    if(is_priv_toggle)is_priv_toggle->connect("toggled",callable_mp(this,&LobbyRoom::set_lock));

    set_up_lobby();
    rebuild_player_list();
    if (load_sc) load_sc->hide_loading();
}


void LobbyRoom::on_leave_pressed()
{

    if(load_sc) load_sc->show_loading("Back To Lobbies");
    if(net_man) net_man->leave_lobby();
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_list.tscn");

    
}

void LobbyRoom::on_lobby_closed()
{
    if(load_sc) load_sc->show_loading("Back To Lobbies");
    if(error_sc) error_sc->createPopup("Host Left The Lobby");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_list.tscn");
}

void LobbyRoom::on_disconnect()
{
    if(load_sc)load_sc->hide_loading();
    if(error_sc)error_sc->createPopup("Disconnected..Servers down");
    get_tree()->change_scene_to_file("res://UserInterfaces/MainMenu/main_menu.tscn");
}

void LobbyRoom::set_up_lobby()
{

    if(!net_man)return;
    Dictionary lobby_data;
    lobby_data = net_man->get_curr_lobb();
    my_id = lobby_data.get("id", 0);
    max_player = lobby_data.get("maxPlayers", 0);     
    bool is_host = (my_id == 1);
    if (lobby_name) lobby_name->set_text(lobby_data.get("name", ""));
    if (lobby_code) lobby_code->set_text("CODE: " + String(lobby_data.get("code", "")));

    if (on_start_butt) on_start_butt->set_visible(is_host);
    if (waiting_text)  waiting_text->set_visible(!is_host);
    if (is_priv_toggle)
    {
        is_priv_toggle->set_pressed_no_signal(lobby_data.get("isPrivate", false));
        is_priv_toggle->set_disabled(!is_host);
    }

    players.clear();
    Dictionary me;
    me["name"] = lobby_data.get("playerName", "");
    me["character"] = lobby_data.get("character", 0);
    players[my_id] = me;

    Array peers = lobby_data.get("peers", Array());
    for (int i = 0; i < peers.size(); i++)
    {
        Dictionary p = peers[i];
        int id = p.get("id", 0);
        Dictionary info;
        info["name"] = p.get("name", "");
        info["character"] = p.get("character", 0);
        players[id] = info;
    }

}

void LobbyRoom::rebuild_player_list()
{
    if (!player_rows_body || row_player.is_null()) return;

    for (int i = 0; i < player_rows_body->get_child_count(); i++)
        player_rows_body->get_child(i)->queue_free();

    Array ids = players.keys();
    for (int i = 0; i < ids.size(); i++)
    {
        int id = ids[i];
        Dictionary info = players[id];

        int c = info.get("character", 0);
        if (c < 0 || c >= char_textures.size()) c = 0;
        Ref<Texture2D> icon;
        if (char_textures.size() > 0) icon = char_textures[c];

        Node* node = row_player->instantiate();
        PlayerRow* row = Object::cast_to<PlayerRow>(node);
        if (!row) { node->queue_free(); continue; }
        player_rows_body->add_child(row);
        row->add_data(id, info.get("name", ""), icon, my_id == 1);
        row->connect("request_to_kick", callable_mp(this, &LobbyRoom::on_kick_requested));
    }


    if (curr_player_count)
        curr_player_count->set_text(String::num_int64(players.size()) + "/" + String::num_int64(max_player));
}

void LobbyRoom::on_player_joined(int id, const String& name, int character)
{
    Dictionary info;
    info["name"] = name;
    info["character"] = character;
    players[id] = info;         
    rebuild_player_list(); 
}

void LobbyRoom::on_player_left(int id)
{
    players.erase(id);          
    rebuild_player_list(); 
}

void LobbyRoom::set_lock(bool lock)
{
    if(net_man)net_man->set_locked(lock);
}

void LobbyRoom::on_lock_changed(bool lock)
{
    if(is_priv_toggle)is_priv_toggle->set_pressed_no_signal(lock);
}

void LobbyRoom::on_kick_requested(int id)
{
    if(net_man)net_man->kick_player(id);
}

void LobbyRoom::on_kicked()
{
    if(load_sc)load_sc->show_loading("Back To Lobbies");
    if(error_sc)error_sc->createPopup("You Where Kicked By The Host");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_list.tscn");
}