#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "LobbyBrowser.h"
#include "UIs/LoadingScreenGUI/loading_screen.h"
#include "UIs/ErrorPopupGUI/ErrorPopup.h"
#include "Network/network_manager.h"
#include "UIs/LobbyRow/lobby_row.h"
#include "UIs/CreateLobbyGUI/CreateLobby.h"
#include "Characters/characters.h"


using namespace godot;




void LobbyBrowser::_bind_methods(){}

void LobbyBrowser::_ready(){
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    create_lobby_sc = get_node<CreateLobby>("%CreateLobby");
    lobby_body = get_node<VBoxContainer>("%LobbyList");
    empty_listbody= get_node<Label>("%EmptyList");
    name_inp = get_node<LineEdit>("%NameInput");
    char_images = get_node<TextureRect>("%CharacterPreview");
    code_inp = get_node<LineEdit>("%CodeInput");
    row_scene = ResourceLoader::get_singleton()->load("res://UserInterfaces/Lobbies/LobbyRow.tscn");
    if (row_scene.is_null()) UtilityFunctions::push_error("LobbyBrowser: δεν φορτώθηκε το LobbyRow.tscn");
    error_sc = get_node<ErrorPopup>("/root/ErrorPopup");
    load_sc = get_node<LoadingScreen>("/root/LoadingScreen");
    net_man = get_node<NetworkManager>("/root/NetworkManagerGlobal");
    Timer* refresh_timer = get_node<Timer>("%RefreshTimer");
    Button* ref_time_butt = get_node<Button>("%RefreshButton");
    Button* Leave_butt = get_node<Button>("%LeaveButton");
    Button* create_lobb_butt = get_node<Button>("%CreateLobbyButton");
    Button* code_join_butt = get_node<Button>("%JoinCodeButton");
    Button* mv_left_butt = get_node<Button>("%LeftButton");
    Button* mv_right_butt = get_node<Button>("%RightButton");


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
    char_index = 0;
    if(char_textures.size() >0 ) set_char_image();


    if(net_man){
        net_man->connect("lobbies_received",callable_mp(this,&LobbyBrowser::on_lobbies_received));
        net_man->connect("disconnected",callable_mp(this,&LobbyBrowser::on_disconnected));
        net_man->connect("created_lobby",callable_mp(this,&LobbyBrowser::on_created_lobby));
        net_man->connect("server_error",callable_mp(this,&LobbyBrowser::on_server_error));
    }  
    Leave_butt->connect("pressed",callable_mp(this,&LobbyBrowser::on_leave_butt_press));
    ref_time_butt->connect("pressed",callable_mp(this,&LobbyBrowser::refresh_lobbylist));
    if (refresh_timer) refresh_timer->connect("timeout", callable_mp(this, &LobbyBrowser::refresh_lobbylist));
    if(mv_left_butt) mv_left_butt->connect("pressed",callable_mp(this,&LobbyBrowser::sw_char_Left));
    if(mv_right_butt) mv_right_butt->connect("pressed",callable_mp(this,&LobbyBrowser::sw_char_right));
    if(create_lobb_butt)create_lobb_butt->connect("pressed",callable_mp(this,&LobbyBrowser::on_create_butt_press));    
    if(create_lobby_sc) create_lobby_sc->connect("create_requested", callable_mp(this, &LobbyBrowser::on_create_requested));
    if(code_join_butt)code_join_butt->connect("pressed",callable_mp(this,&LobbyBrowser::on_code_join_butt_press));
    refresh_lobbylist();
}


void LobbyBrowser::refresh_lobbylist(){if(net_man)net_man->request_lobbies();}

void LobbyBrowser::on_lobbies_received(const Array& list)
{
    if(load_sc)load_sc->hide_loading();
    if(!lobby_body || !empty_listbody) return;
    for (int i = 0; i < lobby_body->get_child_count(); i++)
    {
        Node* child = lobby_body->get_child(i);
        if (child != empty_listbody) child->queue_free();
    }
    if(list.size() <= 0)
    {
        empty_listbody->show();
    }
    else
    {
        empty_listbody->hide();
        if (row_scene.is_null()) return;
        for(int i=0;i < list.size();i++)
        {
            Dictionary lobby = list[i];
            Node* node = row_scene->instantiate();                   
            LobbyRow* row = Object::cast_to<LobbyRow>(node);         
            if (!row) { node->queue_free(); continue; }              
            lobby_body->add_child(row);                              
            row->add_data(lobby);   
            row->connect("join_requested", callable_mp(this, &LobbyBrowser::on_join_req));
        }
    }
}

void LobbyBrowser::on_created_lobby(const Dictionary& msg)
{
    if (load_sc) load_sc->show_loading("Entering lobby");
    get_tree()->change_scene_to_file("res://UserInterfaces/Lobbies/lobby_room.tscn");
}

void LobbyBrowser::on_server_error(const String& reason)
{
    if(load_sc)load_sc->hide_loading();
    if(error_sc)error_sc->createPopup(reason);
    refresh_lobbylist();
}

void LobbyBrowser::on_disconnected()
{
    if(load_sc)load_sc->hide_loading();
    if(error_sc)error_sc->createPopup("Disconnected..Servers down");
    get_tree()->change_scene_to_file("res://UserInterfaces/MainMenu/main_menu.tscn");
}

void LobbyBrowser::on_leave_butt_press()
{
    if(net_man)net_man->disc_from_serv();
    get_tree()->change_scene_to_file("res://UserInterfaces/MainMenu/main_menu.tscn");
}

void LobbyBrowser::set_char_image()
{
    Ref<Texture2D> tex = char_textures[char_index];
    if(char_images)char_images->set_texture(tex);
}

void LobbyBrowser::sw_char_Left()
{
    if(char_textures.size() <= 0) return;
    char_index = (char_index - 1 + char_textures.size()) % char_textures.size();
    set_char_image();
}

void LobbyBrowser::sw_char_right()
{
    if(char_textures.size() <= 0) return;
    char_index = (char_index + 1) % char_textures.size();
    set_char_image();
}

void LobbyBrowser::on_create_butt_press()
{
    if(create_lobby_sc)create_lobby_sc->open();
}

void LobbyBrowser::on_join_req(const String& code,bool from_list)
{
    if (code.strip_edges().is_empty()) return;
    if (load_sc) load_sc->show_loading("Joining lobby");
    if (net_man && name_inp) net_man->join_lobby(code.strip_edges(), name_inp->get_text(),char_index,from_list);
}

void LobbyBrowser::on_code_join_butt_press()
{
    if(code_inp) on_join_req(code_inp->get_text(),false);
}

void LobbyBrowser::on_create_requested(const String& lobby_name,const int& max_players,const bool& is_private)
{
    if(load_sc)load_sc->show_loading("Creating Lobby");
    if(net_man && name_inp)net_man->create_lobby(lobby_name,max_players,is_private,name_inp->get_text(),char_index);
}


