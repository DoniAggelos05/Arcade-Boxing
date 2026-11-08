#include "player_row.h"
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void PlayerRow::_bind_methods(){
    ADD_SIGNAL(MethodInfo("request_to_kick",PropertyInfo(Variant::INT , "player_id")));
}

void PlayerRow::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    char_img = get_node<TextureRect>("%CharacterIcon");
    plr_name_text = get_node<Label>("%PlayerNameText");
    plr_badge = get_node<Label>("%PlayerBadge");
    kick_plr_butt = get_node<Button>("%KickButton");
    if(kick_plr_butt) kick_plr_butt->connect("pressed",callable_mp(this,&PlayerRow::on_kick_butt_press));
}

void PlayerRow::on_kick_butt_press()
{
    emit_signal("request_to_kick",player_id);
}

void PlayerRow::add_data(int id, const String& name, const Ref<Texture2D>& icon, bool i_am_host)
{
    if(char_img && plr_name_text && plr_badge && kick_plr_butt)
    {
        player_id = id;
        plr_name_text->set_text(name);
        char_img->set_texture(icon);
        if(id==1)
        {
            plr_badge->set_text("Host");
            plr_badge->add_theme_color_override("font_color",Color(0.7,0.7,0.0,1.0));
        }
        else
        {
            plr_badge->set_text("Player");
            plr_badge->add_theme_color_override("font_color",Color(1.0,1.0,1.0,1.0));
        }
        kick_plr_butt->set_visible(i_am_host && id != 1);
    }

}