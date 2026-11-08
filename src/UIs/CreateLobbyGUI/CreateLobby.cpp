#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/check_button.hpp>
#include <godot_cpp/classes/engine.hpp>

#include "CreateLobby.h"

using namespace godot;


void CreateLobby::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("create_requested",
        PropertyInfo(Variant::STRING, "lobby_name"),
        PropertyInfo(Variant::INT,"max_players"),
        PropertyInfo(Variant::BOOL,"is_private")));

}

void CreateLobby::open()
{
    amount_in_int = 2;
    turn_int_to_text();
    if (lobb_name_inp) lobb_name_inp->set_text("");
    if (is_priv_toggle) is_priv_toggle->set_pressed(false);
    show();
}
void CreateLobby::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    amount_in_int = 2;
    Button* leave_cr_lobb_butt = get_node<Button>("%LeaveLobbyCrButton");
    Button* minus_butt = get_node<Button>("%MinusButton");
    Button* plus_butt = get_node<Button>("%PlusButton");
    Button* cr_butt = get_node<Button>("%CreateButton");
    is_priv_toggle = get_node<CheckButton>("%PrivateToggle");
    lobb_name_inp = get_node<LineEdit>("%LobbyNameInput");
    player_amount_txt = get_node<Label>("%CountText");
    turn_int_to_text();
    if(leave_cr_lobb_butt) leave_cr_lobb_butt->connect("pressed",callable_mp(this,&CreateLobby::back_to_brows_press));
    if(minus_butt)minus_butt->connect("pressed",callable_mp(this,&CreateLobby::left_butt_press));
    if(plus_butt)plus_butt->connect("pressed",callable_mp(this,&CreateLobby::right_butt_press));
    if(cr_butt)cr_butt->connect("pressed",callable_mp(this,&CreateLobby::send_create_req));

}

void CreateLobby::left_butt_press()
{
    amount_in_int--;
    if(amount_in_int < 2) amount_in_int = 2;
    turn_int_to_text();
}

void CreateLobby::right_butt_press()
{
    amount_in_int++;
    if(amount_in_int > 10) amount_in_int = 10;
    turn_int_to_text();
}


void CreateLobby::turn_int_to_text()
{
    if(player_amount_txt) player_amount_txt->set_text(String::num_int64(amount_in_int)); 
}

void CreateLobby::back_to_brows_press()
{
    hide();
}



void CreateLobby:: send_create_req()
{
    if (!lobb_name_inp || !is_priv_toggle) return;
    emit_signal("create_requested",lobb_name_inp->get_text(),amount_in_int,is_priv_toggle->is_pressed());
    hide();
}