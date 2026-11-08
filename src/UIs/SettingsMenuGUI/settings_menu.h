#pragma once
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/scroll_container.hpp>


namespace godot{

class SettingsMenu: public Control{
GDCLASS(SettingsMenu,Control);

protected:
    static void _bind_methods();

public:
    void _ready() override;

private:
//we will need the texts to be able to scroll till those labels with the buttons
    Label* graphics_sett_text = nullptr;
    Label* sound_sett_text = nullptr;
    ScrollContainer* sett_scroll = nullptr;
    void on_graphbutt_pressed();
    void on_soundbutt_pressed();
    void on_closebutt_pressed();

};


}