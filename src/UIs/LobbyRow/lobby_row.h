#pragma once
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/panel_container.hpp>

namespace godot{

class LobbyRow: public PanelContainer{
GDCLASS(LobbyRow,PanelContainer);

protected:
    static void _bind_methods();
public:
    void _ready() override;
    void add_data(const Dictionary& data);

private:
    Label* name_text = nullptr;
    Label* players_amount = nullptr;
    String code;
    Button* join_butt = nullptr;
    void _on_join_butt_press();
};

}