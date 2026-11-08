#pragma once

#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/texture2d.hpp>


namespace godot{

class PlayerRow: public PanelContainer
{
GDCLASS(PlayerRow,PanelContainer);

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void add_data(int id, const String& name, const Ref<Texture2D>& icon, bool i_am_host);
private:
    int player_id = 0;
    TextureRect* char_img = nullptr;
    Label* plr_name_text = nullptr;
    Label* plr_badge = nullptr;
    Button* kick_plr_butt = nullptr;
    void on_kick_butt_press();
};

}