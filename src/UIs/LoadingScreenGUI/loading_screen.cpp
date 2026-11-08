#include "loading_screen.h"
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/math.hpp>


using namespace godot;


void LoadingScreen::_bind_methods(){};

void LoadingScreen::_ready()
{
    if(Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    loadScreen = get_node<Control>("%Screen");
    loadText = get_node<Label>("%LoadingText");
    movingDots = get_node<Label>("%Dots");
    spinner = get_node<TextureRect>("%Spinner");

    set_process(false);

    
}

void LoadingScreen::_process(double delta)
{
    if(spinner)spinner->set_rotation(spinner->get_rotation() + Math::TAU * delta);
    if(movingDots){
        dots_timer +=  delta;
        if(dots_timer >= 0.4)
        {
            dots_timer = 0.0;
            dot_count = (dot_count + 1) % 4;
            movingDots->set_text(String(".").repeat(dot_count));
        }
    }
}

void LoadingScreen::show_loading(const String& text)
{
    if(loadText && loadScreen)
    {
        loadText->set_text(text);
        dots_timer = 0.0;
        dot_count = 0;
        loadScreen->show();
        set_process(true);
    }
}

void LoadingScreen::hide_loading()
{
    if(loadScreen)loadScreen->hide();
    set_process(false);
}
