#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "settings_menu.h"
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/scroll_container.hpp>

using namespace godot;

void SettingsMenu::_bind_methods(){ADD_SIGNAL(MethodInfo("closed"));};

void SettingsMenu::_ready()
{
    //for when game hasnt started
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    graphics_sett_text = get_node<Label>("%GraphicsSettText");
    sound_sett_text = get_node<Label>("%SoundSettText");
    sett_scroll = get_node<ScrollContainer>("%Scroll");
    Button* graphs_sett_butt = get_node<Button>("%GraphicsButton");
    Button* sound_sett_butt = get_node<Button>("%SoundButton");
    Button* close_butt = get_node<Button>("%CloseButton");
    graphs_sett_butt->connect("pressed",callable_mp(this,&SettingsMenu::on_graphbutt_pressed));
    sound_sett_butt->connect("pressed",callable_mp(this,&SettingsMenu::on_soundbutt_pressed));
    close_butt->connect("pressed",callable_mp(this,&SettingsMenu::on_closebutt_pressed));


}

void SettingsMenu::on_graphbutt_pressed()
{
    if(sett_scroll && graphics_sett_text) sett_scroll->set_v_scroll((int)graphics_sett_text->get_position().y);
}

void SettingsMenu::on_soundbutt_pressed()
{
    if(sett_scroll && sound_sett_text) sett_scroll->set_v_scroll((int)sound_sett_text->get_position().y);
}

void SettingsMenu::on_closebutt_pressed()
{
    hide();
    this->emit_signal("closed");
}


