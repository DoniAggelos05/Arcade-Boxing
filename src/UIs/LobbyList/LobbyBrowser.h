#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include "UIs/LoadingScreenGUI/loading_screen.h"
#include "UIs/ErrorPopupGUI/ErrorPopup.h"
#include "Network/network_manager.h"
#include "UIs/CreateLobbyGUI/CreateLobby.h"


namespace godot{

class LobbyBrowser: public Control
{
 GDCLASS(LobbyBrowser,Control);
protected:
    static void _bind_methods();
public:
    void _ready() override;
private:
    CreateLobby* create_lobby_sc = nullptr;
    VBoxContainer* lobby_body = nullptr;
    Label* empty_listbody = nullptr;
    LineEdit* name_inp = nullptr;
    TextureRect* char_images = nullptr;
    int char_index = 0;
    Array char_textures;
    LoadingScreen* load_sc = nullptr;
    ErrorPopup* error_sc = nullptr;
    NetworkManager* net_man = nullptr;
    Ref<PackedScene> row_scene;
    LineEdit* code_inp = nullptr;

    void sw_char_Left();
    void sw_char_right();    
    void on_leave_butt_press();
    void on_create_butt_press();
    void refresh_lobbylist();
    void on_lobbies_received(const Array& list);
    void on_created_lobby(const Dictionary& msg);
    void on_disconnected();
    void on_code_join_butt_press();
    void on_join_req(const String& code,bool from_list);
    void set_char_image();
    void on_create_requested(const String& lobby_name,const int& max_players,const bool& is_private);    
    void on_server_error(const String& reason);
};


}