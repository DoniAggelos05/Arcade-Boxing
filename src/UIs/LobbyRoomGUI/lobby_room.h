#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/check_button.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

#include "UIs/LoadingScreenGUI/loading_screen.h"
#include "UIs/ErrorPopupGUI/ErrorPopup.h"
#include "Network/network_manager.h"

namespace godot{

class LobbyRoom: public Control
{
GDCLASS(LobbyRoom,Control);

protected:
    static void _bind_methods();

public:
    void _ready() override;

private:
    int my_id = 0;
    int max_player = 0;
    Dictionary players;
    Label* lobby_name = nullptr;
    Label* curr_player_count = nullptr;
    Label* lobby_code = nullptr;
    Label* waiting_text = nullptr;
    CheckButton* is_priv_toggle = nullptr;
    Button* on_start_butt = nullptr;
    VBoxContainer* player_rows_body = nullptr;
    Ref<PackedScene> row_player;
    ErrorPopup* error_sc = nullptr;
    LoadingScreen* load_sc = nullptr;
    NetworkManager* net_man = nullptr;
    Array char_textures;



    void set_up_lobby();//gia oles tis times pou prpeei na boune apeuthias opws px lobby name curr player count lobby code is priv klp ola se ena
    void rebuild_player_list(); //kathe fora pou bainei paixths tha ginetai auto gia na enhmerwnontai oi othones oson htan eidh mesa sto lobby 
    void on_disconnect();
    void set_lock(bool lock);
    void on_lock_changed(bool lock);
    void on_start_press(); //otan pathsei o host to start koumbi
    void on_leave_pressed();
    void on_lobby_closed();
    void on_player_joined(int id, const String& name, int character);
    void on_player_left(int id);
    void on_kick_requested(int id);
    void on_kicked();

};


}