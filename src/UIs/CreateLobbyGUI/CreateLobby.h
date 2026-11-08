#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/check_button.hpp>


namespace godot
{

class CreateLobby: public Control
{
GDCLASS(CreateLobby,Control);

protected:
    static void _bind_methods();
public:
    void _ready() override;
    void open();
private:
    LineEdit* lobb_name_inp = nullptr;
    Label* player_amount_txt = nullptr;
    CheckButton* is_priv_toggle = nullptr;
    int amount_in_int = 2;
    void left_butt_press();
    void right_butt_press();
    void back_to_brows_press();
    void turn_int_to_text();
    void send_create_req();
};

}